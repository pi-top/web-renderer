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
      }
    }
    stage ('Build') {
      agent {
        label 'rpi-cross-compile'
      }
      stages {
        stage ( 'Clean and restore checkout' ) {
        }
        stage ('Install QT license') {
        }
        stage ('Mount cross compiler image') {
        }
        stage ('Download cross compiler') {
        }
        stage ('Build cross compiled project build') {
        }
        stage ('Test') {
          steps {
            dir ('src/build') {
              stash includes: PKG_NAME, name: 'build-' + PKG_NAME, useDefaultExcludes: false
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
                  unstash 'build-' + PKG_NAME
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
