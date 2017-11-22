FROM debian:stretch

ARG WD="/srv"
ARG SRC="${WD}/src"

RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y socat && \
    useradd -s /bin/nologin chall

WORKDIR $WD

# Compilation
RUN apt-get install -y gcc gcc-multilib # change order for dev

RUN dpkg --add-architecture i386 &&\
    apt-get update &&\
    apt-get install -y libstdc++6:i386 libgcc1:i386 build-essential
ADD src/ "$SRC/"
RUN gcc "${SRC}/messager.c" -o messager -m32 -fno-stack-protector && \
    rm -rf "$SRC" && \
    apt-get purge -y --auto-remove gcc


# Preparation

EXPOSE 5555

# prepare flag
ENV DEFAULT_PASSWD="THC18{tmp_default_flag_when_build_image}"

CMD socat TCP-LISTEN:5555,fork EXEC:"/srv/messager",su=chall,stderr
