pipeline {
  agent any
  options {
    // using the Timestamper plugin we can add timestamps to the console log
    timestamps()
    // skip default checkout since creating aptly server can't pull and doesn't need to
    skipDefaultCheckout true
  }
  environment {
    REPO_NAME = "${env.JOB_NAME.split('/')[1]}"
    PKG_NAME  = REPO_NAME.substring(0, REPO_NAME.length() - 4)
  }
  stages {
    stage ('Checkout') {
      agent { label 'master' }
      steps {
        checkoutSubmodule()
        dir (PKG_NAME) {
          //preCommit()
          stash name: 'checkout', includes: '*/**'
        }
      }
    }
    stage ('Build') {
      agent {
        label 'rpi-cross-compile'
      }
      stages {
        stage ( 'Clean and restore checkout' ) {
          steps {
            sh "sudo rm -rf *"
            unstash 'checkout'
          }
        }
        stage ('Install QT license'){
          steps {
            installQtLicence()
          }
        }
        stage ('Mount cross compiler image'){
          steps {
            mountCrossCompilerImage()
          }
        }
        stage ('Download cross compiler'){
          steps {
            downloadCrossCompiler()
          }
        }
        stage ('Build cross compiled project build'){
          steps {
            sh '''
              sudo chroot /mnt <<EOF
                # Temporarily add Debian repo
                apt-get update
                apt-get install -y dirmngr

                echo \"deb http://deb.debian.org/debian buster main\" >> /etc/apt/sources.list
                apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 04EE7237B7D453EC
                apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 648ACFD622F3D138
                apt-key adv --keyserver keyserver.ubuntu.com --recv-keys DCC9EFBF77E11517
                
                apt-get update

                # Run dependencies (probably not necessary)
                apt-get install -y \
                  libqt5quick5 \
                  libqt5qml5 \
                  qml-module-qtquick-window2 \
                  qml-module-qtquick2 \
                  qml-module-qtquick-controls \
                  qml-module-qtwebengine

                # Build dependencies
                apt-get install -y \
                  qt5-default \
                  qtwebengine5-dev

                # Clean up Debian repo (just in case)
                sed -i \"/deb.debian.org/d\" /etc/apt/sources.list
                apt-get update
EOF
            '''

            buildCrossCompiledProject(env.WORKSPACE + "/src/build", "pt-web-ui.pro", "pt-web-ui", "linux-rasp-pi3-g++", true, true)
          }
        }
        stage ('Test') {
          steps {
//            sh '''
//              sudo chroot /mnt <<EOF
//                /tmp/test-pt-os-setup
//EOF
//            '''

            dir ('src/build') {
              stash includes: "pt-web-ui", name: 'build-pt-web-ui', useDefaultExcludes: false
            }
          }
        }

      }
    }
    stage ('Package') {
      agent { label 'master' }
      stages {
        stage ('Package') {
          steps {
            script {
              dir(PKG_NAME) {
                  unstash 'build-pt-web-ui'
              }
              env.DEB_BUILD_OPTIONS="nostrip"
              buildGenericPkg(false, PKG_NAME)
              try {
                lintian()
              } catch (e) {
                currentBuild.result = 'UNSTABLE'
              }
            }
          }
        }
        stage ('Publish') {
          steps {
            publishSirius()
          }
        }
      }
    }
  }
}
