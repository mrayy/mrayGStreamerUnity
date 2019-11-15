#!/bin/bash

DOCKER_GST_UNITY_BRIDGE="/home/mrayGStreamerUnity"

function copyRepo
{
	echo " copyRepo"
  declare -a arr=(Plugin/Android
                  Plugin/sources
                  Plugin/includes
                  Plugin/Externals/gstreamer/Project/Android
                 )

  cd ${MOUNT_GST_UNITY_BRIDGE}
  find -type d -exec mkdir -p "$DOCKER_GST_UNITY_BRIDGE/{}" \;
  for i in "${arr[@]}"
  do
     cp -r ${MOUNT_GST_UNITY_BRIDGE}/$i $DOCKER_GST_UNITY_BRIDGE/$(dirname $i)
  done
	echo "done copyRepo"
}

function buildmray
{
	echo "buildmray"
  # it HAVE TO be subdirectory of ${MOUNT_OUTPUT_PATH}
  OUTPUT_PATH=${MOUNT_OUTPUT_PATH}/mray
  cd $DOCKER_GST_UNITY_BRIDGE/Plugin/Android/mrayGST
  buildAPK release $OUTPUT_PATH
  cd bin/classes
  jar cvf mray.jar org/*
  cp mray.jar $OUTPUT_PATH
  cd ../../
  cp -r libs/${NDK_APP_ABI} $OUTPUT_PATH
	echo "done buildmray"
}

function buildAPK
{
	echo "buildAPK"
  BUILD_TARGET=$1
  BUILD_OUTPUT_PATH=$2

  # be careful with rm command. Be sure that you remove the subdirectory of ${MOUNT_OUTPUT_PATH}
  # it HAVE TO be subdirectory, because remember that someone can give you in config OUTPUT_PATH to "/"
  rm -r $BUILD_OUTPUT_PATH
  mkdir -p $BUILD_OUTPUT_PATH

  android update project -p . -s --target ${NDK_TARGET_LEVEL}
  ndk-build APP_ABI=${NDK_APP_ABI}
  if [ $? -ne 0 ]; then
    echo BUILD FAILED
    exit 1
  fi
  if [ "$COPY_GST_AND_LIB" = true ]; then
    cp ${MOUNT_LIB_GST_ANDROID} libs/${NDK_APP_ABI}/libgstreamer_android.so
  fi
  ant $BUILD_TARGET
	echo "done buildAPK"
}