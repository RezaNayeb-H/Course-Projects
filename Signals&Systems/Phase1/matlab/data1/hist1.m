% EEGLAB history file generated on the 06-Jun-2024
% ------------------------------------------------

EEG.etc.eeglabvers = '2024.0'; % this tracks which version of EEGLAB is being used, you may ignore it
EEG = pop_importdata('dataformat','matlab','nbchan',0,'data','C:\\Users\\jck_5\\Desktop\\myFiles\\Courses\\Term4\\Signals and Systems\\Project\\Phase1\\matlab\\noisy_data1.mat','setname','data1','srate',256,'pnts',0,'xmin',0);
EEG=pop_chanedit(EEG, 'load',{'C:\\Users\\jck_5\\Desktop\\myFiles\\Courses\\Term4\\Signals and Systems\\Project\\Phase1\\matlab\\eeglab_32chan.locs','filetype','autodetect'});
EEG = pop_eegfiltnew(EEG, 'locutoff',1,'plotfreqz',1);
EEG.setname='data1_hpf';
EEG = pop_cleanline(EEG, 'bandwidth',2,'chanlist',[1:32] ,'computepower',1,'linefreqs',50,'newversion',0,'normSpectrum',0,'p',0.01,'pad',2,'plotfigures',0,'scanforlines',0,'sigtype','Channels','taperbandwidth',2,'tau',100,'verb',1,'winsize',4,'winstep',1);
EEG.setname='data1_hpf_cleanLine';
EEG = pop_reref( EEG, []);
EEG.setname='data1_hpf_cleanLine_reref1';
EEG = pop_clean_rawdata(EEG, 'FlatlineCriterion','off','ChannelCriterion','off','LineNoiseCriterion','off','Highpass','off','BurstCriterion',17,'WindowCriterion','off','BurstRejection','off','Distance','Euclidian');
EEG.setname='data1_hpf_cleanLine_reref1_ASR';
EEG = pop_reref( EEG, []);
EEG.setname='data1_hpf_cleanLine_reref2_ASR';
EEG = pop_runica(EEG, 'icatype', 'runica', 'extended',1,'rndreset','yes','interrupt','on','pca',31);
EEG = pop_dipfit_settings( EEG, 'hdmfile','C:\\Users\\jck_5\\Desktop\\myFiles\\Courses\\Term4\\Signals and Systems\\Project\\Phase1\\matlab\\eeglab2024.0\\plugins\\dipfit\\standard_BEM\\standard_vol.mat','mrifile','C:\\Users\\jck_5\\Desktop\\myFiles\\Courses\\Term4\\Signals and Systems\\Project\\Phase1\\matlab\\eeglab2024.0\\plugins\\dipfit\\standard_BEM\\standard_mri.mat','chanfile','C:\\Users\\jck_5\\Desktop\\myFiles\\Courses\\Term4\\Signals and Systems\\Project\\Phase1\\matlab\\eeglab2024.0\\plugins\\dipfit\\standard_BEM\\elec\\standard_1005.elc','coordformat','MNI','coord_transform',[0.25857 -5.5282 -0.26726 0.03062 -0.0018326 0.0011532 1.0992 1.1649 1.1397] );
EEG = pop_multifit(EEG, [1:31] ,'threshold',100,'plotopt',{'normlen','on'});
EEG = pop_iclabel(EEG, 'default');
EEG = pop_subcomp( EEG, [3   4   5   6   7   8  10  11  12  14  15  16  19  20  21  23  25  27  28  29  30  31], 0);
EEG.setname='data1_preprocessed';
EEG = eeg_checkset( EEG );
