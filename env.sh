apply_cmssw_customization_steps() {
    run_cmd git cms-init
    run_cmd git cms-addpkg L1Trigger/CSCTriggerPrimitives
    run_cmd git remote add patrick git@github.com:pkflana/cmssw.git
    run_cmd git fetch patrick
    run_cmd git pull patrick emulator_debug
    run_cmd git cms-addpkg DataFormats/LCTDebug
    run_cmd git cms-addpkg Configuration/Generator
    run_cmd git cms-addpkg Configuration/StandardSequences
    run_cmd git cms-addpkg CondFormats/CSCObjects
    run_cmd git cms-addpkg CalibMuon/CSCCalibration
    run_cmd mkdir -p TPReader
    run_cmd ln -s "$ANALYSIS_PATH/TPReader/TriggerPrimitiveReader" TPReader/TriggerPrimitiveReader
    # run_cmd mkdir -p "$ANALYSIS_PATH/L1Trigger/CSCTriggerPrimitives"
    # run_cmd ln -s L1Trigger/CSCTriggerPrimitives/ "$ANALYSIS_PATH/L1Trigger/CSCTriggerPrimitives/"
}


CMSSW_VERSION="CMSSW_14_1_0_pre5"

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
unset -f action
unset -f apply_cmssw_customization_steps
