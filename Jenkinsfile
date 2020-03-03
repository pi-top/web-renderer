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
    PKG_NAME = "${env.JOB_NAME.split('/')[1]}"
  }
  stages {
    stage ('Build') {
      agent {
        label 'rpi-cross-compile'
      }
      stages {
        stage ( 'Clean' ) {
          steps {
            sh "sudo rm -rf *"
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
            buildCrossCompiledProject(env.WORKSPACE, "pt-web-ui.pro", "pt-web-ui", "linux-rasp-pi3-g++", true, true)
            sh 'cp ${WORKSPACE}/tests/build/test-pt-os-setup /mnt/tmp/'
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
