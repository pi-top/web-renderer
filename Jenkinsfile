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
