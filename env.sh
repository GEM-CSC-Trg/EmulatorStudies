apply_cmssw_customization_steps() {
    run_cmd git cms-init
    run_cmd git cms-addpkg L1Trigger/CSCTriggerPrimitives
    run_cmd git remote add patrick git@github.com:pkflana/cmssw.git # comment if you are patrick
    run_cmd git fetch patrick  # comment if you are patrick
    run_cmd git pull patrick emulator_debug  # comment if you are patrick
    # run_cmd git checkout emulator_debug # uncomment if you are patrick
    run_cmd git cms-addpkg DataFormats/LCTDebug
    run_cmd git cms-addpkg Configuration/Generator
    run_cmd git cms-addpkg Configuration/StandardSequences
    run_cmd git cms-addpkg CondFormats/CSCObjects
    run_cmd git cms-addpkg CalibMuon/CSCCalibration
    run_cmd mkdir -p TPReader
    run_cmd ln -s "$ANALYSIS_PATH/TPReader/TriggerPrimitiveReader" TPReader/TriggerPrimitiveReader
}


CMSSW_VERSION="CMSSW_14_1_0_pre5"

link_cmssw_modules(){ # here go all the links to the CMSSW modules
    mkdir -p cmssw_modules
    mkdir -p "$ANALYSIS_PATH/cmssw_modules/L1Trigger/CSCTriggerPrimitives"
    ln -s $FLAF_CMSSW_BASE/src/L1Trigger/CSCTriggerPrimitives/ "$ANALYSIS_PATH/cmssw_modules/L1Trigger/CSCTriggerPrimitives/"

}
action() {
    local this_file="$( [ ! -z "$ZSH_VERSION" ] && echo "${(%):-%x}" || echo "${BASH_SOURCE[0]}" )"
    local this_dir="$( cd "$( dirname "$this_file" )" && pwd )"
    local this_file_path="$this_dir/$(basename $this_file)"
    export ANALYSIS_PATH="$this_dir"
    echo "Running action with CMSSW version: $CMSSW_VERSION"
    echo source $ANALYSIS_PATH/GEM-CSC-trg-dev/env.sh "$this_file_path" "$CMSSW_VERSION" "$@"
    source $ANALYSIS_PATH/GEM-CSC-trg-dev/env.sh "$this_file_path" "$CMSSW_VERSION" "$@"

}
action "$@"
link_cmssw_modules

unset -f action
unset -f apply_cmssw_customization_steps
unset -f link_cmssw_modules
