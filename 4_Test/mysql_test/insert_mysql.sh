# !/bin/bash

# MYSQL警告：数据库密码暴露


if [ -z "$1" ]; then
    exit 1;
fi

mysql -uroot -pisiem1234! << EOF
    use statistics
    insert into ipsec_tunnel_stat (src_ip, dst_ip) values ('$1', '$2');
    select * from ipsec_tunnel_stat;
EOF




