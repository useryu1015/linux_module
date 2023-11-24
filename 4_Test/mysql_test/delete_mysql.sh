# !/bin/bash

# MYSQL警告：数据库密码暴露

if [ -z "$1" ]; then
    exit 1;
fi

mysql -uroot -pisiem1234! << EOF
    use statistics
    delete from ipsec_tunnel_stat where src_ip='${1}' and dst_ip='${2}';
    select * from ipsec_tunnel_stat;
EOF




