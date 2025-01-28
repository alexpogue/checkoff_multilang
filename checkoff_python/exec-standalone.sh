#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

scratch_container_id=$(docker ps -qf "name=checkoff_python")

if ! [[ -f "${SCRIPT_DIR}/tmp/busybox" ]]; then
  docker run -d --name busybox_tmp busybox:musl sleep 10 > /dev/null
  busybox_container_id=$(docker ps -qf "name=busybox_tmp")
  mkdir -p "${SCRIPT_DIR}/tmp"
  docker cp "$busybox_container_id":/bin/busybox "${SCRIPT_DIR}/tmp/"
  docker kill "$busybox_container_id" > /dev/null
  docker rm "$busybox_container_id" > /dev/null
fi

docker cp ${SCRIPT_DIR}/tmp/busybox "$scratch_container_id":/busybox
docker exec -it "$scratch_container_id" /busybox sh -c '
export PATH="/busybin:$PATH"
/busybox mkdir -p /busybin
/busybox --install /busybin
sh'
