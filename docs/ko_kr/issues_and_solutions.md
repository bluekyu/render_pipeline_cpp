# 문제점 및 해결방법
**Translation**: [English](../issues_and_solutions.md)

## Windows

### 긴 경로 문제
Windows 에서 일반적인 경로 길이는 `MAX_PATH` 값인 260자이다. 이를 넘어서게 되면 기존의 API에서는 인식을 할 수 없다.

이를 해결하는 방법으로 2가지 방법이 있다. 하나는 "extended-length path" 를 사용하는 것이고,
다른 하나는 "Win32 긴 경로 정책" 을 활성화 하는 것이다.

#### Extended-length Path
Extended-length path 는 `\\?\` 를 경로 앞에 추가해주면 된다. 예를 들면 `\\?\C:\very long path` 와 같이 사용한다.
이를 사용하면 최대 32,767 자 까지 인식할 수 있다.

#### Win32 긴 경로 정책
Windows 10 1607 이후 버전부터는 `MAX_PATH` 길이 제한이 사라졌는데, 이러한 정책(policy)을
기본으로 활성화 시키는 것이다.

`HKLM\SYSTEM\CurrentControlSet\Control\FileSystem` 경로에 있는
`LongPathsEnabled` 레지스트리 키를 1로 만들거나,
그룹 정책 편집기에서 다음 경로에 있는 `Enable NTFS long paths` 정책을 활성화 한다.
```
Computer Configuration > Administrative Templates > System > Filesystem > Enable NTFS long paths
```

그리고 프로그램 manifest 를 사용해서 `longPathAware` 태그를 활성화 시키면 되는데, 이 과정은 조금 복잡하다.
(자세한 내용은 References 참조)

대신, Render Pipeline 에서는 CMake 함수를 통해서 manifest 를 자동으로 생성 및 사용할 수 있도록 제공한다.
cmake 폴더에 있는 `windows_utils.cmake` 를 include 시키고 `windows_add_longpath_manifest` 함수를
빌드하려는 target 에 대해서 사용한다.
```
include(windows_utils)

...

windows_add_longpath_manifest(TARGET)
```
그러면 해당 target 의 소스 파일로 "longpath.manifest" 파일을 추가하고 기존처럼 빌드를 수행하면 된다.
CMake 에서는 manifest 파일을 자동으로 인식하여 Visual Studio 에서 manifest 관련 옵션을 설정해준다.

Manifest 설정이 제대로 포함되었는지 확인하려면, 실행 파일을 에디터로 열어서 마지막 부분에서 `longPathAware` 태그가
활성화 되었는지 확인하면 된다.

관련 사용 방법은 [Render Pipeline Samples 프로젝트](https://github.com/bluekyu/rpcpp_samples)에서
CMakeLists.txt 파일들을 확인하면 사용 예시를 볼 수 있다.

#### References
- [MSDN Jeremy Kuhne's Blog: .NET 4.6.2 and long paths on Windows 10](https://blogs.msdn.microsoft.com/jeremykuhne/2016/07/30/net-4-6-2-and-long-paths-on-windows-10/)
- [MSDN Naming Files, Paths, and Namespaces][MSDN Naming Files, Paths, and Namespaces]
- [MSDN Path Field Limits](https://docs.microsoft.com/en-us/cpp/c-runtime-library/path-field-limits)
- [MSDN Application Manifests][Application Manifests]

[MSDN Naming Files, Paths, and Namespaces]: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx
[Application Manifests]: https://msdn.microsoft.com/en-us/library/windows/desktop/aa374191(v=vs.85).aspx
