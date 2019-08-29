FROM conanio:gcc6-armv7hf

RUN mkdir /temp
ADD . /temp/
WORKDIR /temp
RUN printenv
RUN gcc --version

EXPOSE 3000