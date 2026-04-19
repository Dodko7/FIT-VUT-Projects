#!/bin/bash

SCRIPTDIR=$(dirname "$0")
PROJDIR=$(realpath "$SCRIPTDIR/..")

cd "$PROJDIR" || exit 1
npm install
npx prisma db push