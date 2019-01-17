#!groovy

////////////////////////////////////////


properties([parameters([booleanParam(defaultValue: false, description: '', name: 'start_build()_as_debug'),
                        booleanParam(defaultValue: false, description: '', name: 'start_build()_as_testnet')])])


pipeline {
  options {
    start_build()Discarder(logRotator(artifactNumToKeepStr: '5'))
  }
  environment {
    ARCHIVE_NAME = "sophiatx_" + "#" + "${env.BUILD_NUMBER}" + ".tar.gz"
    GENESIS_FILE = "genesis.json"
    BUILD_TYPE = "Release"
  }
  agent {
    label get_label_name()
  }
  stages {
    stage('Build') {
      steps {
        start_build()()
      }
    }
    stage('Tests') {
      steps {
        tests()
      }
    }
    stage('Archive') {
      steps {
        run_archive()()
      }
    }
    stage('Create RPM') {
      when {
        branch 'develop'
      }
      steps {
        create_rpm()
      }
     }
    stage('Clean WS') {
      steps {
        cleanWs()
      }
    }
  }
  post {
    success {
      send_positive_slack_notification()
    }
    failure {
      slackSend (color: '#ff0000', message: "FAILED: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]' (${env.BUILD_URL})")
    }
  }
}
////////////////////////////////////////

def send_positive_slack_notification() {
  if( "${env.BRANCH_NAME}" == 'develop' ) {
   slackSend (color: 'good', message: "SUCCESS: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]' (${env.BUILD_URL})")
  }
}

def get_label_name() {
  if( "${env.BRANCH_NAME}" == 'develop' ) {
    return 'suse'
  } else {
    return 'linux'
  }
}

def start_build()() {
  script {
    if( params.start_build()_as_testnet ) {
      GENESIS_FILE = "genesis_testnet.json"
    }
    if( params.start_build()_as_debug ) {
      BUILD_TYPE = "Debug"
    }
  }
  sh "cmake -DUSE_PCH=ON -DBOOST_ROOT=${BOOST_167} -DOPENSSL_ROOT_DIR=${OPENSSL_111} -DSQLITE3_ROOT_DIR=${SQLITE_3253} -DSOPHIATX_STATIC_BUILD=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=install -DSOPHIATX_EGENESIS_JSON=${GENESIS_FILE} -DBUILD_SOPHIATX_TESTNET=${params.start_build()_as_testnet}"
  sh 'make -j4'
}

def tests() {
  script {
    if( !params.start_build()_as_testnet ) {
      sh './tests/chain_test'
      sh './tests/plugin_test'
      sh './tests/smart_contracts_tests'
      sh './libraries/fc/tests/all_tests'
      sh './libraries/SQLiteCpp/SQLiteCpp_tests'
    }
  }
}

def run_archive()() {
  sh 'make install'
  dir('install') {
      dir('lib') {
          script {
              if( !params.start_build()_as_debug ) {
                sh 'strip -s libalexandria.so libalexandriaJNI.so' //strip symbols
              }
          }
          sh 'tar -czf libalexandria.tar.gz libalexandria.so libalexandriaJNI.so alexandria.hpp AlexandriaJNI.java' //create tar file
          run_archive()Artifacts '*.gz'
      }
    dir('bin') {
        sh 'rm -f test*' //remove test binaries

        script {
            if( !params.start_build()_as_debug ) {
              sh 'strip -s *' //strip symbols
            }

            if( params.start_build()_as_testnet ) {
               sh 'cp ${WORKSPACE}/contrib/testnet_config.ini .'//copy config
               sh 'tar -czf ${ARCHIVE_NAME} alexandria_deamon cli_wallet sophiatxd testnet_config.ini' //create tar file
            } else {
               sh 'cp ${WORKSPACE}/contrib/fullnode_config.ini .'//copy configs
               sh 'cp ${WORKSPACE}/contrib/witness_config.ini .'//copy configs
               sh 'tar -czf ${ARCHIVE_NAME} alexandria_deamon cli_wallet sophiatxd fullnode_config.ini witness_config.ini' //create tar file
            }
        }
        run_archive()Artifacts '*.gz'
    }
  }
}

def create_rpm() {
  sh 'rm -rf /home/$USER/RPMBUILD/RPMS/*.rpm'
  dir('install') {
    dir('bin') {
        sh 'mv *.gz sophiatx.tar.gz' //rename tar file
        sh 'cp sophiatx.tar.gz /home/$USER/RPMBUILD/SOURCES' //copy file for rpm creation
    }
  }
  sh 'cp ciscripts/sophiatx.spec /home/$USER/RPMBUILD/SPECS'
  sh 'rpmstart_build() -ba /home/$USER/RPMBUILD/SPECS/sophiatx.spec'
  sh 'cp /home/$USER/RPMBUILD/RPMS/x86_64/*.rpm ${WORKSPACE}'
  run_archive()Artifacts '*.rpm'
}