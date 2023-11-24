# !/bin/bash

# MYSQL警告：数据库密码暴露

show_mysql() {
    mysql -uroot -pisiem1234! -e "use statistics; select * from ipsec_tunnel_stat \
    where src_ip='${1}' and dst_ip='${2}';"
}

# 示例：update_mysql(local_ip,remote,state,key_life,send_byte)
update_mysql_send() { 
mysql -uroot -pisiem1234! << EOF
    use statistics
    # select * from ipsec_tunnel_stat;
    update ipsec_tunnel_stat set establish=${3}, key_life=${4}, send_bytes='${5}' \
    where src_ip='${1}' and dst_ip='${2}';
    # update ipsec_tunnel_stat set establish=1, key_life=52, send_bytes=53 where src_ip='192.168.100.253' and dst_ip='192.168.100.252';
EOF
    mysql -uroot -pisiem1234! -e "use statistics; select * from ipsec_tunnel_stat \
    where src_ip='${1}' and dst_ip='${2}';"
}


# 示例：update_mysql(local_ip,remote,recv_byte)
update_mysql_recv() {
mysql -uroot -pisiem1234! << EOF
    use statistics
    update ipsec_tunnel_stat set recv_bytes='${3}' \
    where src_ip='${1}' and dst_ip='${2}';
EOF
    mysql -uroot -pisiem1234! -e "use statistics; select * from ipsec_tunnel_stat \
    where src_ip='${1}' and dst_ip='${2}';"
}



# update_mysql_send '192.168.100.253' '192.168.100.252' 0 0 'send'

# MYSQL警告：数据库密码暴露
# mysql -uroot -pisiem1234! -e "use statistics; select * from ipsec_tunnel_stat;"
# mysql -uroot -pisiem1234! -e "use statistics"

# mysql -uroot -pisiem1234! << EOF
#     use statistics
#     select * from ipsec_tunnel_stat;
# EOF
