Name: IPSec
Version: 1.0.0
Release: release
Summary: IPSec application
Summary(zh_CN): IPSec应用程序
AutoReqProv: no
License: EULA
Group: Applications/System
Vendor: 信联科技
Buildarch: x86_64
Source0: %{name}-%{version}.tar.gz
%description
IPSec installation package
%description -l zh_CN
IPSec应用程序安装包

%prep
rm -rf $RPM_BUILD_ROOT/*
echo "RPM_BUILD_ROOT: $RPM_BUILD_ROOT"
%setup -q

%install
echo "%{summary}正在构建"
install -d $RPM_BUILD_ROOT/
cp -a * $RPM_BUILD_ROOT/
exit 0


# %pre: 在安装前运行的脚本
%pre
mkdir -p /xlian
diskMsg=$(df /xlian | grep /)
array=(${diskMsg// / })
freeMb=$((array[3]/1024))
needMb=720
if [[ $needMb -gt ${freeMb} ]] ;then
    output_red "当前计算机磁盘存储空间不足。仅剩余 ${freeMb} Mb。顺利安装运行 IPSec程序需要 $needMb Mb 磁盘空间。"
    echo "退出中……"
    exit 1
else
    echo "当前磁盘剩余${freeMb} Mb。"   >/dev/null 2>&1
fi

# %post：在安装后执行的脚本
%post
output_green() {
  echo -e "\033[32m$1 \033[0m"
}
output_red(){
  echo -e " $1"
}

#mv /xlian/initramfs-4.8.0.img /boot/
if [ -e /boot/initramfs-4.8.0.img ]; then
    rm -rf /boot/initramfs-4.8.0.img*
    rm -rf /boot/System.map-4.8.0*
    rm -rf /boot/vmlinuz-4.8.0*  
fi
rm -rf /lib/modules/4.8.0
mv -f /xlian/System.map-4.8.0 /boot/System.map-4.8.0
mv -f /xlian/vmlinuz-4.8.0 /boot/vmlinuz-4.8.0
mv -f /xlian/4.8.0 /lib/modules/

# 生成引导文件
output_green "内核适配中，等待几分钟"
output_green "wait a moment"
dracut /boot/initramfs-4.8.0.img 4.8.0 -f

output_green "内核初始化..."
if [ -d /sys/firmware/efi ]; then
    grubcfg="/etc/grub2-efi.cfg"
else
    grubcfg="/etc/grub2.cfg"
fi
/usr/sbin/grub2-mkconfig -o ${grubcfg}
/usr/sbin/grub2-set-default 0


# 配置ip_forward
echo "" >> /etc/sysctl.conf
echo \#IPSec_STA >> /etc/sysctl.conf
echo "net.ipv4.ip_forward=1" >> /etc/sysctl.conf
echo \#IPSec_END >> /etc/sysctl.conf
echo "" >> /etc/sysctl.conf

# 配置profile环境变量
echo "" >> /etc/profile
echo \#IPSec_STA >> /etc/profile
echo export PATH=\${PATH}:/xlian/bin:/xlian/mysql/bin:/xlian/script >> /etc/profile
echo export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:/xlian/lib >> /etc/profile
echo \#IPSec_END >> /etc/profile
echo "" >> /etc/profile

# 配置启动项和环境变量
echo "" >> /etc/rc.d/rc.local
echo \#IPSec_STA >> /etc/rc.d/rc.local
echo export PATH=\${PATH}:/xlian/bin:/xlian/mysql/bin:/xlian/script >> /etc/rc.d/rc.local
echo export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:/xlian/lib >> /etc/rc.d/rc.local
echo "service mysqld restart" >> /etc/rc.d/rc.local
echo "sleep 1" >> /etc/rc.d/rc.local
echo "/xlian/script/start_ipsec.sh &" >> /etc/rc.d/rc.local
echo "sleep 1" >> /etc/rc.d/rc.local
echo "/xlian/script/ipsec_update_state.sh &" >> /etc/rc.d/rc.local
echo \#IPSec_END >> /etc/rc.d/rc.local
chmod +x /etc/rc.local


# 配置mysql服务
mkdir /db
cd /xlian/mysql/install
rm -f /var/lib/rpm/.rpm.lock            # mysql脚本会使用rpm命令
source ./mysql.sh
cd /xlian

output_green "应用程序安装完成，文件已释放到目录：/xlian"
echo ""
echo -e "\e[31m请重启设备！(完成4.8内核安装) \e[0m"
echo ""

# 卸载前
%preun
# 2022年6月28日 关闭程序
killall ipsec_update_state.sh
killall start_ipsec.sh
killall ike
mv -f /boot/System.map-4.8.0 /xlian/System.map-4.8.0
mv -f /boot/vmlinuz-4.8.0 /xlian/vmlinuz-4.8.0
mv -f /lib/modules/4.8.0 /xlian/
rm -rf /boot/initramfs-4.8.0.img*         # 2022年6月28日 删除启动引导文件
rm -rf /xlian/config/ipsec                # 2022年6月30日 删除配置文件
rm -rf /db                                # 2022年8月10日 删除mysql配置缓存区
source /xlian/script/del_startup.sh       # 删除启动配置

if [ -d /sys/firmware/efi ]; then
    grubcfg="/etc/grub2-efi.cfg"
else
    grubcfg="/etc/grub2.cfg"
fi
echo "mkconfig: ${grubcfg}"
/usr/sbin/grub2-mkconfig -o ${grubcfg}
/usr/sbin/grub2-set-default 0

# 卸载后
%postun
echo "卸载完成"


# 需要打包的文件
%files
/xlian
%doc

%changelog
* Mon Apr 11 2022 by:xinlian
- first build


