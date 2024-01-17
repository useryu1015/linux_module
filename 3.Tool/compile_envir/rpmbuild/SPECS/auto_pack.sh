#!/bin/bash
echo_red() {
    echo -e "\e[31m$1 \e[0m"
    # echo -e "\033[32m$1 \033[0m"      # green
}

file=IPSec-1.0.0
spec=ipsecFile.spec


# old_dir=$(pwd)
# echo "制作打包文件"
# echo "压缩、制作源码包"
# cd /root/rpmbuild/BUILD
# tar -zcf ${file}.tar.gz ${file}
# mv ${file}.tar.gz /root/rpmbuild/SOURCES
# cd ${old_dir}
# # RPM打包命令
# echo "开始执行打包脚本"
# cd /root/rpmbuild/SPECS
#rpmbuild -bb --nodigest --nosignature ${spec}
echo ""
echo_red "scp 192.168.101.253:/IPSec-1.0.0.tar.gz /root/rpmbuild/SOURCES  "
echo ""
echo "rpmbuild -bb --nodigest --nosignature ${spec}"
echo ""
echo "cp /root/rpmbuild/RPMS/x86_64/IPSec-1.0.0-release.x86_64.rpm /home/project_xlian/"
echo ""
