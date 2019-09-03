FROM conanio/gcc8-armv7hf

LABEL maintainer="Carlos Zoido <carlosz@jfrog.com>"

USER root

RUN apt-get -qq update \
    && apt-get -qq install -y --no-install-recommends \
    openssh-client

RUN eval "$(ssh-agent -s)"

RUN curl -XGET https://api.bintray.com/content/jfrog/jfrog-cli-go/\$latest/jfrog-cli-linux-amd64/jfrog?bt_package=jfrog-cli-linux-amd64 -L -k > /usr/bin/jfrog \
    && chmod u+x /usr/bin/jfrog
