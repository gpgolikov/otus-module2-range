# otus-module1-range

OTUS modile 1 homework 2 - _range_
Probe of _range-v3_ library written by _Eric Niebler_

## installation
```
$ curl -sSL "https://bintray.com/user/downloadSubjectPublicKey?username=bintray" | apt-key add -
$ echo "deb http://dl.bintray.com/gpgolikov/otus-cpp xenial main" | tee -a /etc/apt/sources.list.d/otus.list

$ apt update
$ apt install range
```

## installation of libc++1-7
```
$ curl -sSL "https://build.travis-ci.org/files/gpg/llvm-toolchain-xenial-7.asc" | apt-key add -
$ echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-7 main" | tee -a ${TRAVIS_ROOT}/etc/apt/sources.list >/dev/null

$ apt update
$ apt install libc++1-7 libc++abi-7

```
