# Unicode
**Translation**: [Korean](ko_kr/unicode.md)

## Source Code Example

Note that, on Windows, if the encoding of source codes is UTF8 (not UTF8-BOM),
you may need to add `/utf-8` compiler option.

```cpp
// UTF8 encoding: { 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80, 0x00 }
// MBCS encoding: (ex, cp949) { 0xc7, 0xd1, 0xb1, 0xdb, 0x00 }
char hangul_src_enc[] = "한글";

// UTF16-LE: { 0x5C, 0xD5, 0x00, 0xAE, 0x00, 0x00 }
wchar_t hangul_utf16[] = L"한글";

// UTF8: { 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80, 0x00 }
char hangul_utf8[] = u8"한글";                            // C++11
//char hangul_utf8[] = "\xed\x95\x9c\xea\xb8\x80";      // before C++11

// *** boost::filesystem ******************************************************
// contain UTF16-LE
auto boost_cwd = boost::filesystem::current_path();

// not exist (UTF-8 encoding)
// exist (MBCS encoding)
boost::filesystem::exists(boost::filesystem::path(hangul_src_enc));

// exist (UTF16-LE)
boost::filesystem::exists(boost::filesystem::path(hangul_utf16));

// not exist (UTF8)
boost::filesystem::exists(boost::filesystem::path(hangul_utf8));

// exist (UTF8 to UTF16-LE)
boost::filesystem::exists(boost::filesystem::path(
    boost::locale::conv::utf_to_utf<wchar_t>(hangul_utf8)));

// *** Panda3D Filename *******************************************************
// contain UTF8
Filename filename_cwd = VirtualFileSystem::get_global_ptr()->get_cwd();

// return UTF8
std::string filename_cwd_utf8 = filename_cwd.to_os_specific();

// return UTF16-LE
std::wstring filename_cwd_utf16 = filename_cwd.to_os_specific_w();

// exist (UTF8 encoding)
// not exist (MBCS encoding)
// same as Filename::from_os_specific(hangul_src_enc);
Filename(hangul_src_enc).exists();

// exist (UTF16-LE)
// same as Filename::from_os_specific_w(hangul_utf16);
Filename(hangul_utf16).exists();

// exist (UTF8)
// same as Filename::from_os_specific(hangul_utf8);
Filename(hangul_utf8).exists();
```
