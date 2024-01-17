#!/bin/sh

gather_clean()
{
    cd $RUNDIR/Gather
    make $1 perfix=$INSTALL/xlian
    cd $RUNDIR
}

gather_build()
{
    cd $RUNDIR/Gather
    if [ "$1" == "imx" ];then
    # 根据开发版文档， 配置CC和CXX 
    . /opt/fsl-imx-x11/4.1.15-2.0.0/environment-setup-cortexa7hf-neon-poky-linux-gnueabi demo_gcc.sh
    fi
    make distclean
    make CROSS=$1 perfix=$INSTALL/xlian
    cd $RUNDIR
}

lcd_clean()
{
    cd $RUNDIR/LCD
    ./build_lcd.sh distclean
    cd $RUNDIR
}

lcd_build()
{
    # lcd_clean
    
    cd $RUNDIR/LCD
    ./build_lcd.sh $core
    cd $RUNDIR
}


pack_copy()
{
    cd $INSTALL
    #Fixme：目前只存在imx，CROSS未生效
    cp -ardf $RUNDIR/RunEnv/$CROSS/* ./xlian
    tar -cvf wdd_install.tar.gz ./xlian

    #Fixme: 修改LCD的Makfile
    cp -ardf $RUNDIR/LCD/lcd_wdd ./xlian/bin
    cd $RUNDIR
}

# start
RUNDIR=`cd $(dirname $0); pwd -P`
INSTALL=$RUNDIR/install
core=$1


if [[ -z "$1" ]]; then
    echo "Usage: $0 {imx|x86|clean|distclean}"
    exit
elif [ "$1" == "clean" ];then
    gather_clean $1
    lcd_clean
    exit
elif [ "$1" == "distclean" ];then
    gather_clean $1
    lcd_clean
    rm $INSTALL -rf
    exit
else
    mkdir -p $INSTALL
    gather_build $1
    # lcd_build
    pack_copy
    echo "pack done"
    echo $INSTALL
    ls $INSTALL
fi
