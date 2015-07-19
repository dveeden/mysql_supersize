Settings
--------

MariaDB 10.1:

    INSTALL PLUGIN blackhole SONAME 'ha_blackhole.so';

    max_long_data_size=4294967295 # 4GB
    max_allowed_packet=1073741824 # 1GB
    innodb_log_file_size=20g      # 2x20GB = 40GB (insert must fit in 10%, this allows for 4G transactions)

MySQL 5.6:

    max_allowed_packet=1073741824 # 1GB
    innodb_log_file_size=5g       # 2x5GB = 10GB (insert must fit in 10%, this allows for 1G transactions)

Related Bugs
------------

Oracle MySQL:
* http://bugs.mysql.com/bug.php?id=77781

MariaDB:
* https://mariadb.atlassian.net/browse/MDEV-7481
