import os,re,plistlib,shutil,zipfile,sys
I=os.path.dirname(os.path.abspath(__file__));S=os.path.join(I,'..','web')
APP=os.path.join(I,'build','Payload','CreatorHub.app')
if os.path.isdir(os.path.join(I,'build')): shutil.rmtree(os.path.join(I,'build'))
os.makedirs(os.path.join(APP,'fonts'))
# page: same app, fonts bundled locally, iOS viewport + safe areas
html=open(os.path.join(S,'index.html'),encoding='utf-8').read()
html=re.sub(r'<link rel="preconnect"[^>]*>\n?','',html)
html=re.sub(r'<link rel="stylesheet" href="https://fonts.googleapis.com[^>]*>\n?','',html)
head=('<!doctype html>\n<html lang="en"><head>\n<meta charset="utf-8">\n'
 '<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, viewport-fit=cover">\n'
 '<meta name="format-detection" content="telephone=no">\n<link rel="stylesheet" href="fonts.css">\n'
 '<style>:root{padding-top:env(safe-area-inset-top,0px);padding-bottom:env(safe-area-inset-bottom,0px)}'
 'body{margin:0;font-size:14px;-webkit-text-size-adjust:100%}img{max-width:100%}[hidden]{display:none!important}'
 '*{-webkit-touch-callout:none}input,textarea,[contenteditable]{-webkit-user-select:text;user-select:text}</style>\n')
open(os.path.join(APP,'index.html'),'w',encoding='utf-8').write(head+html+'\n</html>\n')
shutil.copy(os.path.join(I,'fonts.css'),APP)
for f in os.listdir(os.path.join(I,'fonts')): shutil.copy(os.path.join(I,'fonts',f),os.path.join(APP,'fonts'))
for f in os.listdir(os.path.join(I,'icons')): shutil.copy(os.path.join(I,'icons',f),APP)
shutil.copy(os.path.join(I,'CreatorHub'),APP); os.chmod(os.path.join(APP,'CreatorHub'),0o755)
alts=sorted({f.split('@')[0] for f in os.listdir(os.path.join(I,'icons')) if not f.startswith('AppIcon')})
info={
 'CFBundleDevelopmentRegion':'en','CFBundleExecutable':'CreatorHub','CFBundleIdentifier':'com.smore.creatorhub',
 'CFBundleInfoDictionaryVersion':'6.0','CFBundleName':'Creator Hub','CFBundleDisplayName':'Creator Hub',
 'CFBundlePackageType':'APPL','CFBundleShortVersionString':'1.0','CFBundleVersion':'1',
 'CFBundleSupportedPlatforms':['iPhoneOS'],'DTPlatformName':'iphoneos','DTPlatformVersion':'17.0','DTSDKName':'iphoneos17.0',
 'MinimumOSVersion':'15.0','LSRequiresIPhoneOS':True,'UIDeviceFamily':[1,2],'UIRequiredDeviceCapabilities':['arm64'],
 'UILaunchScreen':{},'UIStatusBarStyle':'UIStatusBarStyleDefault','UIViewControllerBasedStatusBarAppearance':True,
 'UISupportedInterfaceOrientations':['UIInterfaceOrientationPortrait'],
 'UISupportedInterfaceOrientations~ipad':['UIInterfaceOrientationPortrait','UIInterfaceOrientationPortraitUpsideDown','UIInterfaceOrientationLandscapeLeft','UIInterfaceOrientationLandscapeRight'],
 'NSPhotoLibraryUsageDescription':'Choose pictures for your app icon, profile and video thumbnails.',
 'NSCameraUsageDescription':'Take a photo for your app icon, profile or video thumbnails.',
 'CFBundleIcons':{'CFBundlePrimaryIcon':{'CFBundleIconFiles':['AppIcon60x60']},
   'CFBundleAlternateIcons':{a:{'CFBundleIconFiles':[a]} for a in alts}},
 'CFBundleIcons~ipad':{'CFBundlePrimaryIcon':{'CFBundleIconFiles':['AppIcon60x60','AppIcon76x76','AppIcon83.5x83.5']},
   'CFBundleAlternateIcons':{a:{'CFBundleIconFiles':[a]} for a in alts}},
}
with open(os.path.join(APP,'Info.plist'),'wb') as f: plistlib.dump(info,f,fmt=plistlib.FMT_BINARY)
open(os.path.join(APP,'PkgInfo'),'w').write('APPL????')
ipa=os.path.join(I,'build','CreatorHub.ipa')
with zipfile.ZipFile(ipa,'w',zipfile.ZIP_DEFLATED) as z:
    for root,_,files in os.walk(os.path.join(I,'build','Payload')):
        for fn in files:
            full=os.path.join(root,fn);arc=os.path.relpath(full,os.path.join(I,'build'))
            zi=zipfile.ZipInfo(arc);zi.compress_type=zipfile.ZIP_DEFLATED
            zi.external_attr=((0o100755 if fn=='CreatorHub' else 0o100644)<<16)
            z.writestr(zi,open(full,'rb').read())
print('alternates',len(alts),'ipa',os.path.getsize(ipa))
