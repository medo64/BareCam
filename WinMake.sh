#!/bin/bash

QT_PATH='/c/Qt'
CERTIFICATE_THUMBPRINT="e9b444fffb1375ece027e40d8637b6da3fdaaf0e"
TIMESTAMP_URL="http://timestamp.digicert.com"


if [ -t 1 ]; then
    ESCAPE_RESET="\E[0m"
    ESCAPE_INFO="\E[36;1m"
    ESCAPE_WARNING="\E[33;1m"
    ESCAPE_ERROR="\E[31;1m"
    ESCAPE_RESULT="\E[32;1m"
fi

if [[ "$2" != "" ]]; then
    echo "Invalid argument count!" >&2
    exit 1
fi

case $1 in
    '')        BUILD='release' ; DO_PACKAGE=0 ;;
    'clean')   BUILD=''        ; DO_PACKAGE=0 ;;
    'debug')   BUILD='debug'   ; DO_PACKAGE=0 ;;
    'release') BUILD='release' ; DO_PACKAGE=0 ;;
    'package') BUILD='release' ; DO_PACKAGE=1 ;;

    *)
        echo "Invalid operation!" >&2
        exit 1
        ;;
esac


DIST_NAME='barecam'
DIST_VERSION=`grep VERSION src/BareCam.pro | head -1 | cut -d'=' -f2 | awk '{print $$1}' | tr -d '"' | xargs`


CMD_QMAKE=`ls $QT_PATH/**/**/bin/qmake.exe | sort | tail -1`
CMD_MAKE=`ls $QT_PATH/Tools/**/bin/mingw32-make.exe | sort | tail -1`

if [[ ! -f "$CMD_QMAKE" ]]; then
    echo -e "${ESCAPE_ERROR}Cannot find qmake!${ESCAPE_RESET}" >&2
    exit 1
fi
QMAKE_DIR=`dirname $CMD_QMAKE`

if [[ ! -f "$CMD_MAKE" ]]; then
    echo -e "${ESCAPE_ERROR}Cannot find make!${ESCAPE_RESET}" >&2
    exit 1
fi

OPENSSL_DIR=$QT_PATH/Tools/QtCreator/bin
if [[ ! -f "$OPENSSL_DIR/libcrypto-1_1-x64.dll" ]] || [[ ! -f "$OPENSSL_DIR/libssl-1_1-x64.dll" ]]; then
    echo -e "${ESCAPE_WARNING}Cannot find OpenSSL files.${ESCAPE_RESET}" >&2
fi


CMD_CERTUTIL=`command -v certutil`
if [[ ! -f "$CMD_CERTUTIL" ]]; then
    echo -e "${ESCAPE_WARNING}Cannot find certutil!${ESCAPE_RESET}" >&2
    CERTIFICATE_THUMBPRINT=""
elif [[ "$CERTIFICATE_THUMBPRINT" == "" ]]; then
    echo -e "${ESCAPE_WARNING}No signing certificate thumbprint!${ESCAPE_RESET}" >&2
else
    $CMD_CERTUTIL -silent -verifystore -user My $CERTIFICATE_THUMBPRINT > /dev/null
    if [[ $? -ne 0 ]]; then
        echo -e "${ESCAPE_WARNING}Cannot validate certificate thumbprint!${ESCAPE_RESET}" >&2
        CERTIFICATE_THUMBPRINT=""
    fi
fi

CMD_SIGNTOOL=""
for SIGNTOOL_PATH in "/c/Program Files (x86)/Microsoft SDKs/ClickOnce/SignTool/signtool.exe" \
            "/c/Program Files (x86)/Windows Kits/10/App Certification Kit/signtool.exe" \
            "/c/Program Files (x86)/Windows Kits/10/bin/x86/signtool.exe"; do
    if [[ -f "$SIGNTOOL_PATH" ]]; then
        CMD_SIGNTOOL="$SIGNTOOL_PATH"
        break
    fi
done


CMD_WINRAR=""
for WINRAR_PATH in "/c/Program Files/WinRAR/WinRAR.exe"; do
    if [[ -f "$WINRAR_PATH" ]]; then
        CMD_WINRAR="$WINRAR_PATH"
        break
    fi
done


echo -e "QMake directory ...: ${ESCAPE_INFO}$QMAKE_DIR${ESCAPE_RESET}"
echo -e "QMake executable ..: ${ESCAPE_INFO}$CMD_QMAKE${ESCAPE_RESET}"
echo -e "Make executable ...: ${ESCAPE_INFO}$CMD_MAKE${ESCAPE_RESET}"
if [[ -f "$CMD_SIGNTOOL" ]]; then
    echo -e "SignTool executable: ${ESCAPE_INFO}$CMD_SIGNTOOL${ESCAPE_RESET}"
else
    echo -e "SignTool executable: ${ESCAPE_WARNING}Not found!${ESCAPE_RESET}"
fi
if [[ -f "$CMD_WINRAR" ]]; then
    echo -e "WinRAR executable .: ${ESCAPE_INFO}$CMD_WINRAR${ESCAPE_RESET}"
else
    echo -e "WinRAR executable .: ${ESCAPE_WARNING}Not found!${ESCAPE_RESET}"
fi


HAS_UNCOMMITTED_RESULT=`git diff --quiet ; echo $?`


rm bin/* 2> /dev/null
rm -r bin/platforms 2> /dev/null
rm -r bin/styles 2> /dev/null
mkdir -p bin
rm -R build/ 2> /dev/null
mkdir -p build
cd build


if [[ "$BUILD" != "" ]]; then
    PATH=$PATH:`dirname $CMD_MAKE`

    $CMD_QMAKE -spec win32-g++ CONFIG+=$BUILD ../src/BareCam.pro
    if [[ $? -ne 0 ]]; then
        echo -e "${ESCAPE_ERROR}QMake failed!${ESCAPE_RESET}" >&2
        exit 1
    fi

    $CMD_MAKE -f Makefile
    if [[ $? -ne 0 ]]; then
        echo -e "${ESCAPE_ERROR}Make failed!${ESCAPE_RESET}" >&2
        exit 1
    fi


    case $BUILD in
        'release')
            cp release/barecam.exe                 ../bin/BareCam.exe
            cp $QMAKE_DIR/libgcc_s_seh-1.dll       ../bin/
            cp $QMAKE_DIR/libstdc++-6.dll          ../bin/
            cp $QMAKE_DIR/libwinpthread-1.dll      ../bin/
            cp $QMAKE_DIR/Qt5Core.dll              ../bin/
            cp $QMAKE_DIR/Qt5Gui.dll               ../bin/
            cp $QMAKE_DIR/Qt5Multimedia.dll        ../bin/
            cp $QMAKE_DIR/Qt5MultimediaWidgets.dll ../bin/
            cp $QMAKE_DIR/Qt5Network.dll           ../bin/
            cp $QMAKE_DIR/Qt5Widgets.dll           ../bin/

            cp $OPENSSL_DIR/libcrypto-1_1-x64.dll ../bin/
            cp $OPENSSL_DIR/libssl-1_1-x64.dll    ../bin/

            mkdir ../bin/mediaservice
            cp $QMAKE_DIR/../plugins/mediaservice/dsengine.dll ../bin/mediaservice/
            cp $QMAKE_DIR/../plugins/mediaservice/qtmedia_audioengine.dll ../bin/mediaservice/

            mkdir ../bin/platforms
            cp $QMAKE_DIR/../plugins/platforms/qwindows.dll ../bin/platforms/

            mkdir ../bin/styles
            cp $QMAKE_DIR/../plugins/styles/qwindowsvistastyle.dll ../bin/styles/

            if [[ "$CERTIFICATE_THUMBPRINT" != "" ]] && [[ -f "$CMD_SIGNTOOL" ]]; then
                echo
                if [[ "$TIMESTAMP_URL" != "" ]]; then
                    "$CMD_SIGNTOOL" sign -s "My" -sha1 $CERTIFICATE_THUMBPRINT -tr $TIMESTAMP_URL -v ../bin/BareCam.exe
                else
                    "$CMD_SIGNTOOL" sign -s "My" -sha1 $CERTIFICATE_THUMBPRINT -v ../bin/BareCam.exe
                fi
            fi

            if [[ $HAS_UNCOMMITTED_RESULT -ne 0 ]] && [[ "$BUILD" == 'release' ]]; then
                echo -e "${ESCAPE_WARNING}Uncommitted changes present.${ESCAPE_RESET}" >&2
            fi

            echo -e "${ESCAPE_RESULT}Release build completed.${ESCAPE_RESET}" >&2

            if [[ $DO_PACKAGE -ne 0 ]]; then
                echo

                INNOSETUP_PATH='/c/Program Files (x86)/Inno Setup 6/ISCC.exe'

                if [[ ! -f "$INNOSETUP_PATH" ]]; then
                    echo -e "${ESCAPE_ERROR}Cannot find InnoSetup 6!${ESCAPE_RESET}" >&2
                    exit 1
                fi

                cd ..
                "$INNOSETUP_PATH" package/win/BareCam.iss
                if [[ $? -eq 0 ]]; then
                    LAST_PACKAGE=`ls -t dist/*.exe | head -1`

                    if [[ "$CERTIFICATE_THUMBPRINT" != "" ]] && [[ -f "$CMD_SIGNTOOL" ]]; then
                        echo
                        if [[ "$TIMESTAMP_URL" != "" ]]; then
                            "$CMD_SIGNTOOL" sign -s "My" -sha1 $CERTIFICATE_THUMBPRINT -tr $TIMESTAMP_URL -v $LAST_PACKAGE
                        else
                            "$CMD_SIGNTOOL" sign -s "My" -sha1 $CERTIFICATE_THUMBPRINT -v $LAST_PACKAGE
                        fi
                    fi

                    echo
                    echo -e "${ESCAPE_RESULT}Package created ($LAST_PACKAGE).${ESCAPE_RESET}" >&2
                else
                    echo -e "${ESCAPE_ERROR}Packaging failed!${ESCAPE_RESET}" >&2
                    exit 1
                fi

                # make ZIP
                ZIP_NAME="$DIST_NAME-$DIST_VERSION.zip"
                "$CMD_WINRAR" a -afzip -ep -m5 dist/$ZIP_NAME bin/*
                if [[ $? -eq 0 ]]; then
                    echo -e "${ESCAPE_RESULT}Package created ($ZIP_NAME).${ESCAPE_RESET}" >&2
                else
                    echo -e "${ESCAPE_ERROR}Packaging failed ($ZIP_NAME)!${ESCAPE_RESET}" >&2
                    exit 1
                fi
            fi
            ;;

        'debug')
            cp debug/barecam.exe                   ../bin/BareCam.exe
            cp $QMAKE_DIR/libgcc_s_seh-1.dll       ../bin/
            cp $QMAKE_DIR/libstdc++-6.dll          ../bin/
            cp $QMAKE_DIR/libwinpthread-1.dll      ../bin/
            cp $QMAKE_DIR/Qt5Core.dll              ../bin/
            cp $QMAKE_DIR/Qt5Gui.dll               ../bin/
            cp $QMAKE_DIR/Qt5Multimedia.dll        ../bin/
            cp $QMAKE_DIR/Qt5MultimediaWidgets.dll ../bin/
            cp $QMAKE_DIR/Qt5Network.dll           ../bin/
            cp $QMAKE_DIR/Qt5Widgets.dll           ../bin/

            cp $OPENSSL_DIR/libcrypto-1_1-x64.dll ../bin/
            cp $OPENSSL_DIR/libssl-1_1-x64.dll    ../bin/

            mkdir ../bin/mediaservice
            cp $QMAKE_DIR/../plugins/mediaservice/dsengine.dll ../bin/mediaservice/
            cp $QMAKE_DIR/../plugins/mediaservice/qtmedia_audioengine.dll ../bin/mediaservice/

            mkdir ../bin/platforms
            cp $QMAKE_DIR/../plugins/platforms/qwindows.dll ../bin/platforms/

            mkdir ../bin/styles
            cp $QMAKE_DIR/../plugins/styles/qwindowsvistastyle.dll ../bin/styles/

            echo -e "${ESCAPE_RESULT}Debug build completed.${ESCAPE_RESET}" >&2
            ;;
    esac
fi