properties([
    parameters ([
        string(name: 'BUILD_NODE', defaultValue: 'omar-build', description: 'The build node to run on'),
        string(name: 'MAKE_VERBOSE', defaultValue: 'VERBOSE=true', description: ''),
        string(name: 'OSSIM_GIT_BRANCH', defaultValue: 'dev', description: 'Used to specify the git branch we are building and executing'),
        booleanParam(name: 'BUILD_WITH_FORTIFY', defaultValue: false, description: 'Check to build and scan source using HP Fortify tool'),
        booleanParam(name: 'CLEAN_WORKSPACE', defaultValue: true, description: 'Clean the workspace at the end of the run')
    ]),
    pipelineTriggers([
            [$class: "GitHubPushTrigger"]
    ]),
    [$class: 'GithubProjectProperty', displayName: '', projectUrlStr: 'https://github.com/ossimlabs/ossim-csm-plugin'],
    buildDiscarder(logRotator(artifactDaysToKeepStr: '', artifactNumToKeepStr: '3', daysToKeepStr: '', numToKeepStr: '20')),
    disableConcurrentBuilds()
])

node ("${BUILD_NODE}") {
    try {
        stage("Load Variables"){
            env.WORKSPACE = pwd()
            env.appName = "ossim-csm-plugin"
            env.MAKE_VERBOSE = "${ MAKE_VERBOSE }"
            env.OSSIM_INSTALL_PREFIX = "${ env.WORKSPACE }/ossim"
            env.MSP_INSTALL_DIR = "/usr/local/msp-1.5"
            env.MSP_HOME = "/usr/local/msp-1.5"
            env.CSM_HOME = "/usr/local/csm"

            withCredentials([string(credentialsId: 'o2-artifact-project', variable: 'o2ArtifactProject')]) {
                step ([$class: "CopyArtifact",
                    projectName: o2ArtifactProject,
                    filter: "common-variables.groovy",
                    flatten: true])
            }

            load "common-variables.groovy"
        }

        stage( "Checkout" ) {
            dir( "ossim-csm-plugin" ) {
                git branch: "${ OSSIM_GIT_BRANCH }", 
                url: "${GIT_PUBLIC_SERVER_URL}/ossim-csm-plugin.git",
                credentialsId: "${CREDENTIALS_ID}"
            }
        }

        echo "WORKSPACE: ${ env.WORKSPACE }"

        stage( "Download Artifacts" ) {
            dir( "${ env.WORKSPACE }" ) {

               step ([ $class: "CopyArtifact",
                    projectName: "ossim-multibranch/${ OSSIM_GIT_BRANCH }",
                    filter: "artifacts/ossim-install.tgz",
                    flatten: true ])
            }
        }

        stage( "Build" ) {
            sh """
                export PATH=${PATH}:/opt/HPE_Security/Fortify_SCA_and_Apps_17.20/bin
                echo "WORKSPACE            = ${env.WORKSPACE}"
                echo "OSSIM_INSTALL_PREFIX = ${env.OSSIM_INSTALL_PREFIX}"
                mkdir -p ${ env.OSSIM_INSTALL_PREFIX }
                tar xfz ossim-install.tgz -C ${ env.OSSIM_INSTALL_PREFIX }
                mv ${ env.OSSIM_INSTALL_PREFIX }/install/* ${ env.OSSIM_INSTALL_PREFIX } 
                rm -rf ${ env.WORKSPACE }/build/CMakeCache.txt
                ${ env.WORKSPACE }/ossim-csm-plugin/scripts/buildNative.sh
            """
        }

        stage("Archive"){
            dir("${env.WORKSPACE}/ossim-csm-plugin") {
               sh "tar cvfz ossim-csm-plugin-install.tgz install"
               archiveArtifacts artifacts:"*.tgz"
            }
        }
        stage("Deliver Artifacts"){
            dir("${env.WORKSPACE}/ossim-csm-plugin"){

              step([$class: 'S3BucketPublisher',
                    dontWaitForConcurrentBuildCompletion: false,
                    entries: [
                               [bucket: "o2-delivery/${OSSIM_GIT_BRANCH}/ossim",
                               excludedFile: '',
                               flatten: false,
                               gzipFiles: false,
                               keepForever: false,
                               managedArtifacts: false,
                               noUploadOnFailure: false,
                               selectedRegion: 'us-east-1',
                               showDirectlyInBrowser: true,
                               sourceFile: "*.tgz",
                               storageClass: 'STANDARD',
                               uploadFromSlave: false,
                               useServerSideEncryption: false]],

                    profileName: 'o2-cicd',
                    userMetadata: []])
            }
        }

        try {
            stage('SonarQube analysis') {
                withSonarQubeEnv("${SONARQUBE_NAME}") {
                    // requires SonarQube Scanner for Gradle 2.1+
                    // It's important to add --info because of SONARJNKNS-281
                    sh """
                  pushd ${env.WORKSPACE}/${appName}/
                  sonar-scanner
                  popd
                """
                }
            }
        } catch(e) {
            println(e)
        }

        stage('Fortify SCA') {
            if(BUILD_FORTIFY == "true"){
                dir("${env.WORKSPACE}/build") {
                    withCredentials([[$class: 'UsernamePasswordMultiBinding',
                            credentialsId: 'fortifyCredentials',
                            usernameVariable: 'HP_FORTIFY_USERNAME',
                            passwordVariable: 'HP_FORTIFY_PASSWORD']]) {
                        sh """
                            export PATH=${PATH}:/opt/HPE_Security/Fortify_SCA_and_Apps_17.20/bin
                            sourceanalyzer -64 -b ossimlabs -scan -f fortifyResults-ossim-csm-plugin.fpr         
                            fortifyclient -url ${HP_FORTIFY_URL} -user "${ HP_FORTIFY_USERNAME }" -password "${ HP_FORTIFY_PASSWORD }" uploadFPR -file fortifyResults-ossim-csm-plugin.fpr -project ossim-csm-plugin -version 1.0
                        """
                    }
                }
            }
        }
    } finally {
        stage("Clean Workspace")
                {
                    if ("${CLEAN_WORKSPACE}" == "true")
                        step([$class: 'WsCleanup'])
                }
    }

}
