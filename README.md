# Build

~~~sh
# pacman -S xmlrpc-c qt5-base
$ qmake
$ make release
$ ./qrtorrent
~~~

# Remote rTorrent setup

`nginx.conf` for SSL and basic authentication:

~~~txt
server {
    listen 443 ssl;
    server_name rtorrent.local;
    ssl_certificate /.../rtorrent.crt;
    ssl_certificate_key /.../rtorrent.key;
    location /RPC2 {
        scgi_pass 127.0.0.1:5000;
        include scgi_params;
        auth_basic "private";
        auth_basic_user_file /.../rtorrent.htpasswd;
    }
}
~~~

`.rtorrent.rc`:

~~~txt
scgi_port = localhost:5000
~~~

# There is a lot of work to be done

You can run lighttpd on port 8080 as a regular user in the repo directory using:

~~~sh
$ lighttpd -D -f lighttpd.conf
~~~

~~~sh
$ xmlrpc localhost:8080/RPC2 system.listMethods
~~~

See https://github.com/rakshasa/rtorrent/wiki/RPC-Setup-XMLRPC

Build in debug mode:

~~~sh
$ qmake
$ make debug&&./qrtorrent
~~~
