#!/bin/sh

usage_exit () {
    echo "Usage: cross-install.sh [--dry-run] [--no-remote-install] image|servers|drivers|commands|all host"
    exit 1
}

TARGET_USER=root
TARGET_DIR=/usr/src
SOURCE_DIR=.
SFTP="sftp"
SFTP_PUT="put"
SFTP_SCRIPT="__sftp.script.tmp"
RSH="rsh"

DRY_RUN=0
if [ "$1" = "--dry-run" ]; then
    DRY_RUN=1
    shift
    echo "Running with --dry-run option..."
fi

REMOTE_INSTALL=1
if [ "$1" = "--no-remote-install" ]; then
    REMOTE_INSTALL=0
    shift
    echo "Running with --no-remote-install option..."
fi

if [ "$1" = "" ]; then
    usage_exit
else
    ACTION=$1
    case $ACTION in
    image)
       GREP_ACTION="NO_MATCH"
       INSTALL_CMD=""
       IS_IMAGE=1
       ;;
    servers|drivers|commands)
       GREP_ACTION="$ACTION"
       INSTALL_CMD="cd $TARGET_DIR/$ACTION && make install"
       IS_IMAGE=0
       ;;
    all)
       GREP_ACTION="$SOURCE_DIR"
       INSTALL_CMD="cd $TARGET_DIR/commands && make install &&\
 cd $TARGET_DIR/servers  && make install &&\
 cd $TARGET_DIR/drivers  && make install"
       IS_IMAGE=1
       ;;
    *)
       usage_exit
       ;;
    esac
    shift
fi

if [ "$1" = "" ]; then
    usage_exit
else
    TARGET_HOST=$1
fi

echo "" > $SFTP_SCRIPT

#Process servers|drivers|commands actions
RAW_PATHS=`find $SOURCE_DIR -name \*.raw | grep $GREP_ACTION | grep -v commands/simple`
for RAW_PATH in $RAW_PATHS
do
        BIN_PATH=`echo $RAW_PATH | sed "s/\.raw//"`
        chmod a+x $BIN_PATH
        echo $SFTP_PUT $BIN_PATH $TARGET_DIR/$BIN_PATH >> $SFTP_SCRIPT
        if [ $DRY_RUN -eq 1 ]; then
            echo $SFTP $SFTP_PUT $BIN_PATH $TARGET_DIR/$BIN_PATH
        fi
done

#Process image action
if [ $IS_IMAGE -eq 1 ]; then
    echo $SFTP_PUT $SOURCE_DIR/image.aout /boot/image.aout >> $SFTP_SCRIPT
    if [ $DRY_RUN -eq 1 ]; then
        echo $SFTP $SFTP_PUT $SOURCE_DIR/image.aout /boot/image.aout
    fi
fi

#Run SFTP script
ERR=0
echo $SFTP -b $SFTP_SCRIPT $TARGET_USER@$TARGET_HOST
if [ $DRY_RUN -eq 0 ]; then
    $SFTP -b $SFTP_SCRIPT $TARGET_USER@$TARGET_HOST
    ERR=$?
fi
rm -f $SFTP_SCRIPT
if [ $ERR -gt 0 ]; then
    exit $ERR
fi

if [ $REMOTE_INSTALL -eq 1 ]; then
    #Perform remote install if necessary
    if [ "$INSTALL_CMD" != "" ]; then
        echo "$RSH -l $TARGET_USER $TARGET_HOST \"$INSTALL_CMD\""
        if [ $DRY_RUN -eq 0 ]; then
            $RSH -l $TARGET_USER $TARGET_HOST "$INSTALL_CMD"
        fi
    fi
fi

