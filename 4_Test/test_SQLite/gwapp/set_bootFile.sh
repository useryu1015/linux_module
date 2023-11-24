#!/bin/bash

echo \#IPsec >> /etc/profile
echo export PATH=\${PATH}:/gwapp/bin >> /etc/profile
echo export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:/gwapp/lib >> /etc/profile

echo \#IPsec >> /etc/rc.d/rc.local
echo \source \/gwapp/sh/load_modules.sh \& >> /etc/rc.d/rc.local


