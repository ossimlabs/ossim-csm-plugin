properties([
    parameters ([
        string(name: 'BUILD_NODE', defaultValue: 'omar-build', description: 'The build node to run on'),
        string(name: 'MAKE_VERBOSE', defaultValue: 'VERBOSE=true', description: ''),
        string(name: 'OSSIM_GIT_BRANCH', defaultValue: '', description: 'Used to specify the git branch we are building and executing, uses the current git branch if left blank'),
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
            if ("${OSSIM_GIT_BRANCH}" == "") {
                OSSIM_GIT_BRANCH="${BRANCH_NAME}"
            }

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
                checkout(scm)
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

    } finally {
        stage("Clean Workspace")
                {
                    if ("${CLEAN_WORKSPACE}" == "true")
                        step([$class: 'WsCleanup'])
                }
    }

}
