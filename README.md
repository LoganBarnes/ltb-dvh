LTB Distance Volume Hierarchy
=============================
[![Travis CI][travis-badge]][travis-link]
[![Codecov][codecov-badge]][codecov-link]
[![MIT License][license-badge]][license-link]
[![Docs][docs-badge]][docs-link]

**[Source][source-code-link]** | **[Documentation][documentation-link]**

### Development

Git subtree command reference:

```bash
# All from root dir or things may break

# Before all subtree commands
git remote add -f ltb-whatev git@github.com:LoganBarnes/ltb-whatev.git

# Subtree commands use form: subtree-name remote-name branch-name
git subtree add --prefix ltb-whatev ltb-whatev master --squash
git subtree push --prefix ltb-whatev ltb-whatev master
git subtree pull --prefix ltb-whatev ltb-whatev master --squash
```

Helpful video [tutorial](https://www.youtube.com/watch?v=t3Qhon7burE).


[travis-badge]: https://travis-ci.org/LoganBarnes/ltb-dvh.svg?branch=master
[travis-link]: https://travis-ci.org/LoganBarnes/ltb-dvh
[codecov-badge]: https://codecov.io/gh/LoganBarnes/ltb-dvh/branch/master/graph/badge.svg
[codecov-link]: https://codecov.io/gh/LoganBarnes/ltb-dvh
[license-badge]: https://img.shields.io/badge/License-MIT-blue.svg
[license-link]: https://github.com/LoganBarnes/ltb-dvh/blob/master/LICENSE
[docs-badge]: https://codedocs.xyz/LoganBarnes/ltb-dvh.svg
[docs-link]: https://codedocs.xyz/LoganBarnes/ltb-dvh

[protobuf-link]: https://developers.google.com/protocol-buffers/
[grpc-link]: https://grpc.io/
[magnum-link]: https://magnum.graphics/

[source-code-link]: https://github.com/LoganBarnes/ltb-dvh
[documentation-link]: https://codedocs.xyz/LoganBarnes/ltb-dvh/index.html
