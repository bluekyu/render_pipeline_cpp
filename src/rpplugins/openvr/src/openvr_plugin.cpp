#include "../include/openvr_plugin.hpp"

#include <thread>

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <genericAsyncTask.h>
#include <geomTriangles.h>
#include <geomNode.h>
#include <matrixLens.h>
#include <camera.h>
#include <material.h>
#include <materialAttrib.h>
#include <texture.h>
#include <textureAttrib.h>
#include <geomVertexWriter.h>

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

#include "openvr_render_stage.h"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::OpenVRPlugin)

namespace rpplugins {

std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

// ************************************************************************************************

struct OpenVRPlugin::Impl
{
    Impl(OpenVRPlugin& self);

    void on_stage_setup(void);

    void setup_camera(void);
    bool init_compositor(void) const;
    void setup_render_models(void);
    NodePath setup_render_models(vr::TrackedDeviceIndex_t unTrackedDeviceIndex, bool force=false);
    NodePath load_model(const std::string& model_name);
    NodePath create_mesh(const std::string& model_name, vr::RenderModel_t* render_model, vr::RenderModel_TextureMap_t* render_texture);

    void update_hmd_pose(void);
    void convert_matrix_to_lmatrix(const vr::HmdMatrix34_t& from, LMatrix4f& to) const;
    void convert_matrix_to_lmatrix(const vr::HmdMatrix44_t& from, LMatrix4f& to) const;

public:
    static RequrieType require_plugins_;

    OpenVRPlugin& self_;

    std::shared_ptr<OpenvrRenderStage> render_stage_;
    float distance_scale_ = 1.0f;
    bool update_camera_pose_ = true;
    bool update_eye_pose_ = true;
    bool load_render_model_ = false;

    // vive data
    vr::IVRSystem* HMD_;

    vr::TrackedDevicePose_t tracked_device_pose_[vr::k_unMaxTrackedDeviceCount];

    uint32_t render_width_;
    uint32_t render_height_;

    NodePath render_models_[vr::k_unMaxTrackedDeviceCount];
};

OpenVRPlugin::RequrieType OpenVRPlugin::Impl::require_plugins_;

OpenVRPlugin::Impl::Impl(OpenVRPlugin& self): self_(self)
{
}

void OpenVRPlugin::Impl::on_stage_setup(void)
{
    render_stage_ = std::make_shared<OpenvrRenderStage>(self_.pipeline_);
    render_stage_->set_render_target_size(render_width_, render_height_);
    self_.add_stage(render_stage_);

    distance_scale_ = boost::any_cast<float>(self_.get_setting("distance_scale"));
    update_camera_pose_ = boost::any_cast<bool>(self_.get_setting("update_camera_pose"));
    update_eye_pose_ = boost::any_cast<bool>(self_.get_setting("update_eye_pose"));
    load_render_model_ = boost::any_cast<bool>(self_.get_setting("load_render_model"));

    if (!init_compositor())
    {
        self_.error("Failed to initialize VR Compositor!");
        return;
    }

    setup_render_models();

    self_.debug("Finish to initialize OpenVR.");
}

void OpenVRPlugin::Impl::setup_camera(void)
{
    // create OpenVR lens and copy from original lens.
    PT(MatrixLens) vr_lens = new MatrixLens;
    *DCAST(Lens, vr_lens) = *rpcore::Globals::base->get_cam_lens();
    vr_lens->set_interocular_distance(0);
    vr_lens->set_convergence_distance(0);
    rpcore::Globals::base->get_cam_node()->set_lens(vr_lens);

    const LMatrix4f z_to_y = LMatrix4f::convert_mat(CS_zup_right, CS_yup_right);
    LMatrix4f proj_mat;

    // left
    convert_matrix_to_lmatrix(HMD_->GetProjectionMatrix(vr::Eye_Left, vr_lens->get_near(), vr_lens->get_far()), proj_mat);
    vr_lens->set_left_eye_mat(z_to_y * proj_mat * vr_lens->get_film_mat_inv());

    // FIXME: fix to projection matrix for stereo culling mono projection for culling
    // mono lens (see PerspectiveLens::do_compute_projection_mat)
    proj_mat(2, 0) = 0;
    proj_mat(2, 1) = 0;
    vr_lens->set_user_mat(z_to_y * proj_mat * vr_lens->get_film_mat_inv());

    // right
    convert_matrix_to_lmatrix(HMD_->GetProjectionMatrix(vr::Eye_Right, vr_lens->get_near(), vr_lens->get_far()), proj_mat);
    vr_lens->set_right_eye_mat(z_to_y * proj_mat * vr_lens->get_film_mat_inv());
}

bool OpenVRPlugin::Impl::init_compositor(void) const
{
    vr::EVRInitError peError = vr::VRInitError_None;

    if (!vr::VRCompositor())
    {
        self_.error("Compositor initialization failed.");
        return false;
    }

    return true;
}

void OpenVRPlugin::Impl::setup_render_models(void)
{
    if (!load_render_model_)
        return;

    if (!HMD_)
        return;

    for (uint32_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
    {
        if (!HMD_->IsTrackedDeviceConnected(unTrackedDevice))
            continue;

        NodePath model = setup_render_models(unTrackedDevice);
        if (!model.is_empty())
            model.reparent_to(rpcore::Globals::render);
    }
}

NodePath OpenVRPlugin::Impl::setup_render_models(vr::TrackedDeviceIndex_t unTrackedDeviceIndex, bool force)
{
    if (!force && !load_render_model_)
        return NodePath();

    if (unTrackedDeviceIndex >= vr::k_unMaxTrackedDeviceCount)
        return NodePath();

    // try to find a model we've already set up
    const std::string& model_name = GetTrackedDeviceString(HMD_, unTrackedDeviceIndex, vr::Prop_RenderModelName_String);

    NodePath model = load_model(model_name);
    if (model.is_empty())
    {
        const std::string& tracking_system_name = GetTrackedDeviceString(HMD_, unTrackedDeviceIndex, vr::Prop_TrackingSystemName_String);
        self_.error(std::string("Unable to load render model for tracked device ") + std::to_string((size_t)unTrackedDeviceIndex) + "(" + tracking_system_name + ", " + model_name + ")");
    }
    else
    {
        render_models_[unTrackedDeviceIndex] = model;
    }

    return model;
}

NodePath OpenVRPlugin::Impl::load_model(const std::string& model_name)
{
    vr::RenderModel_t* model = nullptr;
    vr::EVRRenderModelError model_error;
    while (1)
    {
        model_error = vr::VRRenderModels()->LoadRenderModel_Async(model_name.c_str(), &model);
        if (model_error != vr::VRRenderModelError_Loading)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (model_error != vr::VRRenderModelError_None)
    {
        self_.error(std::string("Unable to load render model ") + model_name + " - " + vr::VRRenderModels()->GetRenderModelErrorNameFromEnum(model_error));
        return NodePath();
    }

    vr::RenderModel_TextureMap_t* texture;
    while (1)
    {
        model_error = vr::VRRenderModels()->LoadTexture_Async(model->diffuseTextureId, &texture);
        if (model_error != vr::VRRenderModelError_Loading)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (model_error != vr::VRRenderModelError_None)
    {
        self_.error(std::string("Unable to load render texture for render model ") + model_name);
        vr::VRRenderModels()->FreeRenderModel(model);
        return NodePath();
    }

    NodePath model_np = create_mesh(model_name, model, texture);

    vr::VRRenderModels()->FreeRenderModel(model);
    vr::VRRenderModels()->FreeTexture(texture);

    return model_np;
}

NodePath OpenVRPlugin::Impl::create_mesh(const std::string& model_name, vr::RenderModel_t* render_model, vr::RenderModel_TextureMap_t* render_texture)
{
    // Add vertices
    PT(GeomVertexData) vdata = new GeomVertexData(model_name, GeomVertexFormat::get_v3n3t2(), Geom::UsageHint::UH_static);
    vdata->unclean_set_num_rows(render_model->unVertexCount);

    GeomVertexWriter vertex(vdata, InternalName::get_vertex());
    GeomVertexWriter normal(vdata, InternalName::get_normal());
    GeomVertexWriter texcoord0(vdata, InternalName::get_texcoord());

    for (uint32_t k=0, k_end=render_model->unVertexCount; k < k_end; ++k)
    {
        const auto& pos = render_model->rVertexData[k].vPosition;
        const auto& norm = render_model->rVertexData[k].vNormal;
        const auto& tc = render_model->rVertexData[k].rfTextureCoord;

        vertex.add_data3(pos.v[0], -pos.v[2], pos.v[1]);
        normal.add_data3(norm.v[0], -norm.v[2], norm.v[1]);
        texcoord0.add_data2(tc[0], tc[1]);
    }

    // Add indices
    const size_t triangle_count = render_model->unTriangleCount * 3;

    PT(GeomTriangles) prim = new GeomTriangles(Geom::UsageHint::UH_static);
    prim->reserve_num_vertices(triangle_count);
    for (size_t k = 0, k_end = triangle_count; k < k_end; k+=3)
        prim->add_vertices(render_model->rIndexData[k], render_model->rIndexData[k+1], render_model->rIndexData[k+2]);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    CPT(RenderState) state = RenderState::make_empty();
    PT(Material) pmat = new Material;
    pmat->set_base_color(LColor(0.6f));
    pmat->set_emission(LColor(0));
    pmat->set_refractive_index(1.5f);
    pmat->set_roughness(1.0f);
    pmat->set_metallic(0.0f);
    state = state->add_attrib(MaterialAttrib::make(pmat));

    PT(Texture) texture = new Texture(model_name);
    texture->setup_2d_texture(render_texture->unWidth, render_texture->unHeight, Texture::ComponentType::T_unsigned_byte, Texture::Format::F_rgba8);

    PTA_uchar dest = texture->modify_ram_image();
    const auto src = render_texture->rubTextureMapData;
    for (size_t k=0, k_end=dest.size(); k < k_end; k+=4)
    {
        dest[k+2] = src[k+0];   // b
        dest[k+1] = src[k+1];   // g
        dest[k+0] = src[k+2];   // r
        dest[k+3] = src[k+3];   // a
    }

    texture->set_wrap_u(SamplerState::WM_clamp);
    texture->set_wrap_v(SamplerState::WM_clamp);
    texture->set_magfilter(SamplerState::FT_linear);
    texture->set_minfilter(SamplerState::FT_linear_mipmap_linear);

    state = state->add_attrib(DCAST(TextureAttrib, TextureAttrib::make())->add_on_stage(TextureStage::get_default(), texture));

    PT(GeomNode) geom_node = new GeomNode(model_name);
    geom_node->add_geom(geom, state);

    return NodePath(geom_node);
}

void OpenVRPlugin::Impl::update_hmd_pose(void)
{
    if (!HMD_)
        return;

    static const LMatrix4f z_to_y = LMatrix4f::convert_mat(CS_zup_right, CS_yup_right);
    static const LMatrix4f y_to_z = LMatrix4f::convert_mat(CS_yup_right, CS_zup_right);

    vr::VRCompositor()->WaitGetPoses(tracked_device_pose_, vr::k_unMaxTrackedDeviceCount, NULL, 0);

    if (tracked_device_pose_[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
    {
        NodePath cam = rpcore::Globals::base->get_cam();

        if (update_camera_pose_)
        {
            LMatrix4f hmd_mat;
            convert_matrix_to_lmatrix(tracked_device_pose_[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking, hmd_mat);
            hmd_mat[3][0] *= distance_scale_;
            hmd_mat[3][1] *= distance_scale_;
            hmd_mat[3][2] *= distance_scale_;
            cam.set_mat(z_to_y * hmd_mat * y_to_z);
        }

        if (update_eye_pose_)
        {
            // Update for IPD change.
            LMatrix4f left_eye_mat;
            LMatrix4f right_eye_mat;

            convert_matrix_to_lmatrix(HMD_->GetEyeToHeadTransform(vr::Eye_Left), left_eye_mat);
            convert_matrix_to_lmatrix(HMD_->GetEyeToHeadTransform(vr::Eye_Right), right_eye_mat);

            left_eye_mat[3][0] *= distance_scale_;
            left_eye_mat[3][1] *= distance_scale_;
            left_eye_mat[3][2] *= distance_scale_;

            right_eye_mat[3][0] *= distance_scale_;
            right_eye_mat[3][1] *= distance_scale_;
            right_eye_mat[3][2] *= distance_scale_;

            cam.find("left_eye").set_mat(z_to_y * left_eye_mat * y_to_z);
            cam.find("right_eye").set_mat(z_to_y * right_eye_mat * y_to_z);
        }
    }

    if (!load_render_model_)
        return;

    for (int device_index = vr::k_unTrackedDeviceIndex_Hmd+1; device_index < vr::k_unMaxTrackedDeviceCount; ++device_index)
    {
        if (tracked_device_pose_[device_index].bPoseIsValid && !render_models_[device_index].is_empty())
        {
            LMatrix4f pose_mat;
            convert_matrix_to_lmatrix(tracked_device_pose_[device_index].mDeviceToAbsoluteTracking, pose_mat);
            pose_mat[3][0] *= distance_scale_;
            pose_mat[3][1] *= distance_scale_;
            pose_mat[3][2] *= distance_scale_;
            render_models_[device_index].set_mat(LMatrix4::scale_mat(distance_scale_) * z_to_y * pose_mat * y_to_z);
        }
    }
}

void OpenVRPlugin::Impl::convert_matrix_to_lmatrix(const vr::HmdMatrix34_t& from, LMatrix4f& to) const
{
    to.set(
        from.m[0][0], from.m[1][0], from.m[2][0], 0.0,
        from.m[0][1], from.m[1][1], from.m[2][1], 0.0,
        from.m[0][2], from.m[1][2], from.m[2][2], 0.0,
        from.m[0][3], from.m[1][3], from.m[2][3], 1.0f
    );
}

void OpenVRPlugin::Impl::convert_matrix_to_lmatrix(const vr::HmdMatrix44_t& from, LMatrix4f& to) const
{
    to.set(
        from.m[0][0], from.m[1][0], from.m[2][0], from.m[3][0],
        from.m[0][1], from.m[1][1], from.m[2][1], from.m[3][1],
        from.m[0][2], from.m[1][2], from.m[2][2], from.m[3][2],
        from.m[0][3], from.m[1][3], from.m[2][3], from.m[3][3]
    );
}

// ************************************************************************************************

OpenVRPlugin::OpenVRPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>(*this))
{
}

OpenVRPlugin::~OpenVRPlugin(void)
{
    for (int k = 0; k < vr::k_unMaxTrackedDeviceCount; ++k)
        impl_->render_models_[k].remove_node();
    vr::VR_Shutdown();
}

OpenVRPlugin::RequrieType& OpenVRPlugin::get_required_plugins(void) const 
{
    return impl_->require_plugins_;
}

void OpenVRPlugin::on_load(void)
{
    // Loading the SteamVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;
    impl_->HMD_ = vr::VR_Init(&eError, vr::VRApplication_Scene);

    if (eError != vr::VRInitError_None)
    {
        impl_->HMD_ = nullptr;
        error(std::string("Unable to init VR runtime: ") + vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        return;
    }

    if (!vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError))
    {
        impl_->HMD_ = nullptr;
        vr::VR_Shutdown();

        error(std::string("Unable to get render model interface: ") + vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        return;
    }

    debug(std::string("Driver: ") + driver_string());
    debug(std::string("Display: ") + display_string());

    impl_->setup_camera();

    impl_->HMD_->GetRecommendedRenderTargetSize(&impl_->render_width_, &impl_->render_height_);

    debug(std::string("OpenVR render target size: (") + std::to_string(impl_->render_width_) + ", " + std::to_string(impl_->render_height_) + ")");
}

void OpenVRPlugin::on_stage_setup(void)
{
    impl_->on_stage_setup();
}

void OpenVRPlugin::on_post_render_update(void)
{
    vr::Texture_t leftEyeTexture ={(void*)impl_->render_stage_->get_eye_texture(vr::Eye_Left), vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
    vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

    vr::Texture_t rightEyeTexture ={(void*)impl_->render_stage_->get_eye_texture(vr::Eye_Right), vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

    impl_->update_hmd_pose();
}

vr::IVRSystem* OpenVRPlugin::hmd_instance(void) const
{
    return impl_->HMD_;
}

NodePath OpenVRPlugin::render_model(int device_index) const
{
    if (device_index >= vr::k_unMaxTrackedDeviceCount)
        return NodePath();

    return impl_->render_models_[device_index];
}

const vr::TrackedDevicePose_t& OpenVRPlugin::tracked_device_pose(int device_index) const
{
    if (device_index >= vr::k_unMaxTrackedDeviceCount)
    {
        static const vr::TrackedDevicePose_t invalid;
        return invalid;
    }

    return impl_->tracked_device_pose_[device_index];
}

uint32_t OpenVRPlugin::render_width(void) const
{
    return impl_->render_width_;
}

uint32_t OpenVRPlugin::render_height(void) const
{
    return impl_->render_height_;
}

std::string OpenVRPlugin::driver_string(void) const
{
    return GetTrackedDeviceString(impl_->HMD_, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
}

std::string OpenVRPlugin::display_string(void) const
{
    return GetTrackedDeviceString(impl_->HMD_, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
}

void OpenVRPlugin::set_distance_scale(float distance_scale)
{
    impl_->distance_scale_ = distance_scale;
}

}
