# -*- mode: python -*-

block_cipher = None


a = Analysis(['SAPF_GUI.py'],
             pathex=['D:\\Mitja\\Projekti\\SerijskiFilterEnoF\\Programi\\SAPF_final\\Martin_SAPF\\SAPF_DLOG'],
             binaries=[],
             datas=[],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher)
a.datas += [('Logo_LRTME.png','Logo_LRTME.png', 'Data')]
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          name='SAPF_GUI',
          debug=False,
          strip=False,
          upx=True,
          console=False , icon='Logo_LRTME.ico')
