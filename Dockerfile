FROM debian:stretch

ARG WD="/srv"
ARG SRC="${WD}/src"

RUN dpkg --add-architecture i386 &&\
    apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y socat && \
    useradd -s /bin/nologin chall

WORKDIR $WD

# Compilation

ADD src/ "$SRC/"
RUN apt-get install -y gcc gcc-multilib libstdc++6:i386 libgcc1:i386 &&\
    gcc "${SRC}/messager.c" -o messager -m32 -fno-stack-protector && \
    rm -rf "$SRC" && \
    apt-get purge -y --auto-remove gcc


# Preparation

EXPOSE 5555

# prepare flag
ENV DEFAULT_PASSWD="THC18{tmp_default_flag_when_build_image}"

CMD socat TCP-LISTEN:5555,fork EXEC:"/srv/messager",su=chall,stderr
