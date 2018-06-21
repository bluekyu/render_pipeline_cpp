/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file assimpLoader.h
 * @author rdb
 * @date 2011-03-29
 */

#pragma once

#include "filename.h"
#include "modelRoot.h"
#include "texture.h"
#include "pmap.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Character;
class CharacterJointBundle;
class PartGroup;
class AnimBundle;
class AnimGroup;

namespace rpassimp {

struct char_cmp {
  bool operator () (const char *a, const char *b) const {
    return strcmp(a,b) < 0;
  }
};
typedef pmap<const char *, const aiNode *, char_cmp> BoneMap;
typedef pmap<const char *, PT(Character), char_cmp> CharacterMap;

/**
 * Class that interfaces with Assimp and builds Panda nodes to represent the
 * Assimp structures.  The loader should be reusable.
 */
class AssimpLoader : public TypedReferenceCount
{
public:
    AssimpLoader();
    virtual ~AssimpLoader();

    /**
     * Returns a space-separated list of extensions that Assimp can load, without
     * the leading dots.
     */
    void get_extensions(std::string &ext) const;

    /**
     * Reads from the indicated file.
     */
    bool read(const Filename &filename);

    /**
     * Converts scene graph structures into a Panda3D scene graph, with _root
     * being the root node.
     */
    void build_graph();

public:
    bool _error;
    PT(ModelRoot) _root;
    Filename _filename;
    Mutex _lock;

private:
    Assimp::Importer _importer;
    const aiScene *_scene;

    // These arrays are temporarily used during the build_graph run.
    PT(Texture) *_textures;
    CPT(RenderState) *_mat_states;
    PT(Geom) *_geoms;
    unsigned int *_geom_matindices;
    BoneMap _bonemap;
    CharacterMap _charmap;

    /**
     * Finds a node by name.
     */
    const aiNode *find_node(const aiNode &root, const aiString &name);

    /**
     * Converts an aiTexture into a Texture.
     */
    void load_texture(size_t index);

    /**
     * Converts an aiMaterial into a RenderState.
     */
    void load_texture_stage(const aiMaterial &mat, const aiTextureType &ttype, CPT(TextureAttrib) &tattr);

    /**
     * Converts an aiMaterial into a RenderState.
     */
    void load_material(size_t index);

    /**
     * Creates a CharacterJoint from an aiNode
     */
    void create_joint(Character *character, CharacterJointBundle *bundle, PartGroup *parent, const aiNode &node);

    /**
     * Creates a AnimChannelMatrixXfmTable from an aiNodeAnim
     */
    void create_anim_channel(const aiAnimation &anim, AnimBundle *bundle, AnimGroup *parent, const aiNode &node);

    /**
     * Converts an aiMesh into a Geom.
     */
    void load_mesh(size_t index);

    /**
     * Converts an aiNode into a PandaNode.
     */
    void load_node(const aiNode &node, PandaNode *parent);

    /**
     * Converts an aiLight into a LightNode.
     */
    void load_light(const aiLight &light);
};

}
