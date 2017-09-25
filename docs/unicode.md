# Unicode
**Translation**: [Korean](ko_kr/unicode.md)

## Source Code Example

Note that, on Windows, if the encoding of source codes is UTF8 (not UTF8-BOM),
you may need to add `/utf-8` compiler option.

### String Literals
```cpp
// Source code is UTF8 encoding: { 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80, 0x00 }
// Source code is MBCS encoding: cp949 { 0xc7, 0xd1, 0xb1, 0xdb, 0x00 }
char hangul_src_enc[] = "한글";

// UTF16-LE: { 0x5C, 0xD5, 0x00, 0xAE, 0x00, 0x00 }
wchar_t hangul_utf16[] = L"한글";

// UTF8: { 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80, 0x00 }
char hangul_utf8[] = u8"한글";                                // C++11
//char hangul_utf8[] = "\xed\x95\x9c\xea\xb8\x80";          // before C++11
```

### Boost Filesystem
```cpp
// Windows: contain UTF16-LE    (filesystem::path::value_type is wchar_t)
// Unix:    contain UTF8        (filesystem::path::value_type is char)
auto boost_cwd = boost::filesystem::current_path();

// False (UTF-8 encoding)
// True  (MBCS encoding)
boost::filesystem::path hangul_src_path(hangul_src_enc);
boost::filesystem::exists(hangul_src_path);

// True (UTF16-LE)
boost::filesystem::path hangul_utf16_path(hangul_utf16);
boost::filesystem::exists(hangul_utf16_path);

// False (UTF8)
boost::filesystem::path hangul_utf8_path(hangul_utf8);
boost::filesystem::exists(hangul_utf8_path);

// Wrong conversion: UTF-16 to MBCS cp437 when Windows System Locale is English (cp437)
// Right conversion: UTF-16 to MBCS cp949 when Windows System Locale is Korean (cp949)
auto hangul_utf16_path_string = hangul_utf16_path.string();

// Right conversion: UTF8 to UTF16-LE using boost::locale::conv::utf_to_utf
// when Windows System Locale is English (cp437) and Korean (cp949)
// True
boost::filesystem::path hangul_utf16_conv_path(boost::locale::conv::utf_to_utf<wchar_t>(hangul_utf8));
boost::filesystem::exists(hangul_utf16_conv_path);

#ifdef _WIN32
// Right conversion: UTF-16 to MBCS (cp949) using std::locale
// when Windows System Locale is English (cp437) or Korean (cp949)
{
    auto previous_locale = boost::filesystem::path::imbue(std::locale(".949"));     // set locale to cp949. See MSDN page
    //auto previous_locale = boost::filesystem::path::imbue(std::locale("ko-KR"));  // set locale to ko-KR. See MSDN page

    // filesystem to MBCS (cp949) string
    auto hangul_utf16_path_string = hangul_utf16_path.string();         // NOTE: old creation.

    boost::filesystem::path::imbue(previous_locale);                    // restore
}
#endif

// Right conversion: UTF-16 to UTF8 using boost::locale
// when Windows System Locale is English (cp437) or Korean (cp949)
{
    // set locale to UTF8
    auto previous_locale = boost::filesystem::path::imbue(boost::locale::generator().generate(""));

    // filesystem to UTF8 string
    auto hangul_utf16_path_string = hangul_utf16_path.string();         // NOTE: old creation.

    // filesystem to UTF16 wstring
    boost::filesystem::path new_hangul_utf8_path(hangul_utf8);          // WARN: new creation. (the old generates wrong result)
    auto hangul_utf16_path_wstring = new_hangul_utf8_path.wstring();

    // True (filesystem from UTF8)
    boost::filesystem::exists(new_hangul_utf8_path);

    boost::filesystem::path::imbue(previous_locale);                    // restore
}
```

### Panda3D Filename
```cpp
// contain UTF8
auto filename_cwd = VirtualFileSystem::get_global_ptr()->get_cwd();

// UTF8 string
auto filename_cwd_utf8 = filename_cwd.to_os_specific();

// UTF16-LE wstring
auto filename_cwd_utf16 = filename_cwd.to_os_specific_w();

// True (UTF-8 encoding)
// False (MBCS encoding)
Filename hangul_src_filename(hangul_src_enc);       // same as Filename::from_os_specific(hangul_src_enc);
hangul_src_filename.exists();

// True (UTF16-LE)
Filename hangul_utf16_filename(hangul_utf16);       // same as Filename::from_os_specific_w(hangul_utf16);
hangul_utf16_filename.exists();

// True (UTF8)
Filename hangul_utf8_filename(hangul_utf8);         // same as Filename::from_os_specific(hangul_utf8);
hangul_utf8_filename.exists();
```
