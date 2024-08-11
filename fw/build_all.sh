make clean
make -j8 bl RELEASE=1 BOARD=NEO
make -j8 app RELEASE=1 BOARD=NEO
cd application && make ota RELEASE=1 APP_VERSION=3 BOARD=NEO && cd ..
cd application && make full RELEASE=1 APP_VERSION=3 BOARD=NEO && cd ..