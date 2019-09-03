FROM conanio/gcc8-armv8

LABEL maintainer="Carlos Zoido <carlosz@jfrog.com>"

USER root

RUN apt-get -qq update \
    && apt-get -qq install -y --no-install-recommends \
    openssh-client

RUN eval "$(ssh-agent -s)"


RUN cd /usr/bin && curl -fL https://getcli.jfrog.io | sh
