# KSI Command-line Tool

Guardtime Keyless Signature Infrastructure (KSI) is an industrial scale blockchain platform that cryptographically ensures data integrity and proves time of existence. Its keyless signatures, based on hash chains, link data to global calendar blockchain. The checkpoints of the blockchain, published in newspapers and electronic media, enable long term integrity of any digital asset without the need to trust any system.

There are many applications for KSI, a classical example is signing of any type logs, e.g. system logs, financial transactions, call records. For more, see [https://guardtime.com](https://guardtime.com)

KSI command-line tool enables the access to the KSI blockchain and its functions from the shell. It can be used for signing data in files or other sources, extending existing KSI signatures and verifying them using different trust anchors.


## INSTALLATION

### Latest Release from Guardtime Repository

In order to install the `ksi` CentOS/RHEL packages directly from the Guardtime public repository, download and save the repository configuration to the `/etc/yum.repos.d/` folder:

```
cd /etc/yum.repos.d

# In case of RHEL / CentOS 6
sudo curl -O http://download.guardtime.com/ksi/configuration/guardtime.el6.repo

# In case of RHEL / CentOS 7
sudo curl -O http://download.guardtime.com/ksi/configuration/guardtime.el7.repo

yum install ksi-tools
```

### From Source Code

If the latest version is needed or the package is not available for the platform you are using, check out source code from Github and build it using `gcc` or VS. To build KSI tool `libksi` and `libksi-devel` (KSI C SDK) packages are needed. `libksi` is available in Guardtime repository or as source code in GitHub:
[https://github.com/GuardTime/libksi](https://github.com/GuardTime/libksi).

Use `rebuild.sh` script to build KSI tool on CentOS/RHEL. 

See `WinBuild.txt` to read how to build KSI tool on Windows.

See `test/TEST-README.md` to learn how to run KSI command-line tool tests on Windows and Linux.


### Upgrade

The older package of KSI tool is deprecated but can concurrently exist with `ksi-tools`. After some time the older versions will be obsolated by `ksi-tools`, thus it is strongly recommended to upgrade. To upgrade from `ksitool` one must install package `ksi-tools`.

To perform upgrade of older package of `ksi` or `ksi-tools` call:

```
yum upgrade ksi
yum upgrade ksi-tools
```

## USAGE

In order to get trial access to the KSI platform, go to
[https://guardtime.com/blockchain-developers](https://guardtime.com/blockchain-developers)


The first argument of the tool is the KSI command followed by the KSI service
configuration parameters and options. An example of obtaining a signature for
a data file `data`:

```
ksi sign -i data -S http://example.gateway.com:3333/gt-signingservice
```

See `man ksi` for detailed usage instructions (Linux) or read documentation formatted as pdf from `doc/` directory (Windows).


## LICENSE

See `LICENSE` file.

## CONTRIBUTING

See `CONTRIBUTING.md` file.

## DEPENDENCIES

```
Library   Version    License type  Source

libksi    >=3.12     Apache 2.0    https://github.com/GuardTime/libksi
OpenSSL   >=0.9.8    BSD           https://github.com/openssl/
Curl      >=7.37.0   MIT           https://github.com/curl/curl.git
```

* Note 1: OpenSSL is `libksi` dependency. On Windows platform it's optional.
  This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/). This product includes cryptographic software written by Eric Young (eay@cryptsoft.com). This product includes software written by Tim Hudson (tjh@cryptsoft.com).

* Note 2: `Curl` is `libksi` dependency. On Windows platform it's optional.


## COMPATIBILITY

```
OS/Platform                         Compatibility

RHEL 6 and 7, x86_64 architecture   Fully compatible and tested.
CentOS 6 and 7, x86_64 architecture Fully Compatible and tested.
Debian                              Compatible but not tested on regular basis.
OS X                                Compatible but not tested on regular basis.
Windows 7, 8, 10                    Compatible but not tested on regular basis.
```
