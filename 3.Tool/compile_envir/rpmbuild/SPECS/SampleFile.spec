Name: SampleFile
Version: 1.1.0
Release: release
Summary: SampleFile RPM Setup File
Summary(zh_CN): SampleFile RPM 版
AutoReqProv: no
License: EULA
Group: Applications/System
Vendor: wja
Buildarch: x86_64
Source0: %{name}-%{version}.tar.gz
%description
This is just for importing some files.
%description -l zh_CN
这只是用于导入一些文件。

%prep
rm -rf $RPM_BUILD_ROOT/*
echo "test1"
%setup -q

%install
echo "%{summary}正在构建"
install -d $RPM_BUILD_ROOT/
cp -a * $RPM_BUILD_ROOT/
exit 0


%pre
mkdir -p /opt/wja/files
diskMsg=$(df /opt/wja/files | grep /)
array=(${diskMsg// / })
freeMb=$((array[3]/1024))
needMb=100
if [[ $needMb -gt ${freeMb} ]] ;then
    output_red "当前计算机磁盘存储空间不足。仅剩余 ${freeMb} Mb。顺利安装运行 SampleFile 需要 $needMb Mb 磁盘空间。"
    echo "退出中……"
    exit 1
else
    echo "当前磁盘剩余${freeMb} Mb。"   >/dev/null 2>&1
fi

#echo "test"
#mkdir /rpm_test
#mv /rpm_test /home

%post
output_green() {
  echo -e "\033[32m$1 \033[0m"
}

output_green "文件已经释放到/opt/wja/files"


%preun

%postun

%files
/opt/wja/files
%doc


%changelog
* Mon Feb 28 2022 wangjingan <wja91@qq.cn>
- first build


