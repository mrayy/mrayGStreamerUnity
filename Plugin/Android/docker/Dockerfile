FROM ubuntu:16.04


#A docker with ubuntu:16.04 for Android GUB


#=====================required linux build======================================
RUN apt-get update && apt-get -y install git build-essential pkg-config wget unzip default-jre openjdk-8-jdk file
#===============================================================================


#=====================optional linux build======================================
#GUB Ubuntu dependencies
#RUN apt-get -y install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev libgl1-mesa-dev libgles2-mesa-dev
#GUB test app dependencies
#RUN apt-get -y install libsdl2-dev libglu1-mesa-dev freeglut3-dev
#===============================================================================


#=====================install android ndk=======================================
ARG NDK_TARGET_LEVEL
ARG NDK_APP_ABI

ENV NDK_TARGET_LEVEL=${NDK_TARGET_LEVEL}
ENV NDK_APP_ABI=${NDK_APP_ABI}
ENV ANDROID_NDK_HOME /usr/local/android-ndk-r12b

RUN wget https://dl.google.com/android/repository/android-ndk-r12b-linux-x86_64.zip && \
unzip -q android-ndk-r12b-linux-x86_64.zip -d /usr/local/ && \
rm android-ndk-r12b-linux-x86_64.zip
ENV PATH $PATH:$ANDROID_NDK_HOME
#===============================================================================


#=====================install ant===============================================
ARG ANT_VERSION

ENV ANT_VERSION=${ANT_VERSION}
ENV ANT_HOME=/opt/ant

RUN wget http://archive.apache.org/dist/ant/binaries/apache-ant-${ANT_VERSION}-bin.tar.gz && \
tar -xzf apache-ant-${ANT_VERSION}-bin.tar.gz && \
mv apache-ant-${ANT_VERSION} ${ANT_HOME} && \
rm apache-ant-${ANT_VERSION}-bin.tar.gz
ENV PATH ${PATH}:${ANT_HOME}/bin
#===============================================================================


#=====================install android SDK=======================================
ENV ANDROID_SDK_HOME=/usr/local/android-sdk-linux

RUN wget https://dl.google.com/android/android-sdk_r24.4.1-linux.tgz && \
tar -xzf android-sdk_r24.4.1-linux.tgz -C /usr/local && \
rm  android-sdk_r24.4.1-linux.tgz && \
cd ${ANDROID_SDK_HOME}/tools && ( sleep 4 && while [ 1 ]; do sleep 1; echo y; done ) | ./android update sdk --no-ui -a --filter 1,2,3,4,${NDK_TARGET_LEVEL}
ENV PATH $PATH:$ANDROID_SDK_HOME/tools
#===============================================================================


#=====================get gstreamer for android=================================
ARG GST_ANDROID_ARCH
ARG GST_ANDROID_VERSION

ENV GST_ANDROID_ARCH=${GST_ANDROID_ARCH}
ENV GST_ANDROID_VERSION=${GST_ANDROID_VERSION}
ENV GSTREAMER_ROOT_ANDROID=/usr/local/gstreamer/${GST_ANDROID_ARCH}

RUN SORTED_VERSIONS=$(printf ${GST_ANDROID_VERSION}'\n1.9.90' | sort -V) && \
case "${SORTED_VERSIONS}" in \
1.9.90*) export GST_ANDROID_PART=universal && \
         export GST_ANDROID_PATH=/usr/local/gstreamer ;; \
*      ) export GST_ANDROID_PART=${GST_ANDROID_ARCH} && \
         export GST_ANDROID_PATH=/usr/local/gstreamer/${GST_ANDROID_ARCH} ;; \
esac && \

wget https://gstreamer.freedesktop.org/data/pkg/android/${GST_ANDROID_VERSION}/gstreamer-1.0-android-${GST_ANDROID_PART}-${GST_ANDROID_VERSION}.tar.bz2 && \
mkdir -p ${GST_ANDROID_PATH} && \
tar -xjf gstreamer-1.0-android-${GST_ANDROID_PART}-${GST_ANDROID_VERSION}.tar.bz2 -C ${GST_ANDROID_PATH} && \
rm gstreamer-1.0-android-${GST_ANDROID_PART}-${GST_ANDROID_VERSION}.tar.bz2

RUN sed -i -e 's/#define GST_GL_HAVE_GLSYNC 0/#define GST_GL_HAVE_GLSYNC 1/g' $GSTREAMER_ROOT_ANDROID/lib/gstreamer-1.0/include/gst/gl/gstglconfig.h
RUN sed -i -e 's/#define GST_GL_HAVE_GLUINT64 0/#define GST_GL_HAVE_GLUINT64 1/g' $GSTREAMER_ROOT_ANDROID/lib/gstreamer-1.0/include/gst/gl/gstglconfig.h
RUN sed -i -e 's/#define GST_GL_HAVE_GLINT64 0/#define GST_GL_HAVE_GLINT64 1/g' $GSTREAMER_ROOT_ANDROID/lib/gstreamer-1.0/include/gst/gl/gstglconfig.h

RUN cp $GSTREAMER_ROOT_ANDROID/lib/gstreamer-1.0/include/gst/gl/gstglconfig.h $GSTREAMER_ROOT_ANDROID/include/gstreamer-1.0/gst/gl/
#===============================================================================


#=====================entry point===============================================
COPY ./entrypoint/ /home/docker-entrypoint
RUN sed -i -e 's/\r//g' /home/docker-entrypoint/*.sh
RUN chmod +x /home/docker-entrypoint/*.sh
ENTRYPOINT ["/home/docker-entrypoint/docker-entrypoint.sh"]
#===============================================================================
