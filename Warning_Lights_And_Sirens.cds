<?xml version="1.0" encoding="UTF-8"?>
<distribution version="20.0.0" name="Warning_Lights_And_Sirens" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{D970CC35-7605-41A1-B211-931C6B072B45}">
		<general appName="Warning_Lights_And_Sirens" outputLocation="c:\data\WarningLights_And_Sirens\installer" relOutputLocation="installer" outputLocationWithVars="c:\data\WarningLights_And_Sirens\installer" relOutputLocationWithVars="installer" upgradeBehavior="1" autoIncrement="true" version="1.1.4">
			<arp company="SSC" companyURL="" supportURL="" contact="Mikael Danielsson" phone="+46 70 208 90 60" comments=""/>
			<summary title="Warning Lights and Sirens program installer" subject="" keyWords="" comments="" author="Mikael Danielsson, SSC"/></general>
		<userinterface language="English" showPaths="true" showRuntimeOnly="false" readMe="" license="">
			<dlgstrings welcomeTitle="Warning_Lights_And_Sirens Program" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="Warning_Lights_And_Sirens" dirID="100" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Warning_Lights_And_Sirens" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\data\WarningLights_And_Sirens\WarningLights_And_Sirens.uir" relSourcePath="WarningLights_And_Sirens.uir" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="c:\data\WarningLights_And_Sirens\WarningLights_And_Sirens.ini" relSourcePath="WarningLights_And_Sirens.ini" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="c:\data\WarningLights_And_Sirens\cvibuild.WarningLights_And_Sirens\Release\WarningLights_And_Sirens.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="c:\data\WarningLights_And_Sirens\ADAMTCP.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="100" projectID="0">
				<fileID>2</fileID></projectOutput>
			<projectDependencies dirID="100" projectID="0">
				<fileID>3</fileID></projectDependencies></fileGroups>
		<shortcuts>
			<shortcut name="Warning_Lights_And_Sirens" targetFileID="2" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products>
			<product name="NI LabWindows/CVI Shared Runtime 2020 f3" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{9F4D115F-D443-4FB2-A294-BCE034DB6627}" path="C:\ProgramData\National Instruments\MDF\ProductCache\" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="20.3.49152">
				<cutSoftDeps>
					<softDep name=".NET Support" UC="{0DDB211A-941B-4125-9518-E81E10409F2E}" depKey="ni-cvi-dotnet.CVI2003.RTE"/>
					<softDep name="Analysis Support" UC="{86208B51-159E-4F6F-9C62-0D5EFC9324D8}" depKey="ni-cvi-analysis.CVI2003.RTE"/>
					<softDep name="Network Streams Support" UC="{40A5AD7F-4BAF-4A5C-8B56-426B84F75C05}" depKey="ni-cvi-networkstreams.CVI2003.RTE"/>
					<softDep name="Network Variable Support" UC="{15CE39FE-1354-484D-B8CA-459077449FB3}" depKey="ni-cvi-network-variable.CVI2003.RTE"/>
					<softDep name="TDMS Support" UC="{5A8AF88D-486D-4E30-A7A5-8D8A039BBEBF}" depKey="ni-cvi-tdms.CVI2003.RTE"/></cutSoftDeps></product></products>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="false" cvirte="true" dotnetsup="false" instrsup="true" lowlevelsup="true" lvrt="true" netvarsup="false" rtutilsup="true">
			<hasSoftDeps/>
			<doNotAutoSelect>
			<component>activeXsup</component>
			</doNotAutoSelect></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<exeName>installer.exe</exeName>
			<launchConditions>
				<condition>MINOS_WIN7_SP1</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<baselineProducts>
			<product name="NI LabWindows/CVI Shared Runtime 2020 f3" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{9F4D115F-D443-4FB2-A294-BCE034DB6627}" path="(None)" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="20.3.49152">
				<dependencies>
					<productID>{07298686-C518-4981-9AE1-6E62273BF43A}</productID>
					<productID>{0B476064-821E-4BA8-A0A8-489ECDDAAD55}</productID>
					<productID>{0FFFDCED-4232-4312-BBDB-179F9FDAB7EA}</productID>
					<productID>{192594FE-5397-4F45-8137-A5C487CE3849}</productID>
					<productID>{1C79B8BD-F5F8-4825-8BC8-78098A726AE1}</productID>
					<productID>{28B285EA-90C8-4C3A-94C0-B0196A3FEF07}</productID>
					<productID>{39CB13EF-8677-4FF8-942D-20021AFD7AB6}</productID>
					<productID>{3F36AC37-CC28-4780-AF19-D80ACEDE0323}</productID>
					<productID>{41BDEA25-514E-4BDA-AC47-186E97C9D3F7}</productID>
					<productID>{43C6873A-8548-4944-AF22-8B1C3312A070}</productID>
					<productID>{4C351C60-5ECC-4920-AEB6-DFE60EA14283}</productID>
					<productID>{539F08CF-F2FD-482C-A5ED-65FD2620789D}</productID>
					<productID>{5F52B242-C3C1-46D0-A9D7-E00A0E128C7B}</productID>
					<productID>{62418E99-C730-4D5D-96B2-B4DDB810A652}</productID>
					<productID>{64ECB814-3A6A-4E48-9D2F-D6C2EDD725B7}</productID>
					<productID>{67CFC7DD-BF77-4D45-9AE9-742AA8658AE7}</productID>
					<productID>{69586F66-56DA-43BD-BB8E-A6DAE9A4DF6D}</productID>
					<productID>{77607BC7-8226-47C1-9BC2-140A14333357}</productID>
					<productID>{866E5D39-AB76-4536-B1CA-554BC66A1900}</productID>
					<productID>{9905478F-5785-468A-A35A-D84325D61EB8}</productID>
					<productID>{A12FB799-41BF-4A9A-98F2-2FE4F333B718}</productID>
					<productID>{A59D9F6E-27DE-410C-9820-9D844DC78D05}</productID>
					<productID>{AFC999BB-F270-46EF-B748-AE755EC75322}</productID>
					<productID>{B524884D-C3AC-417A-939E-34A16118A14C}</productID>
					<productID>{C0A9F5E2-DCD5-44C1-8B03-C560F4C06D6C}</productID>
					<productID>{C1771155-665A-4C7F-B85D-D4AACF786223}</productID>
					<productID>{C18DA001-CF46-48A5-8DDB-322EA6DBFB4F}</productID>
					<productID>{D8F71C3E-C0AE-4D69-920E-7D927C950A1A}</productID>
					<productID>{DCCB918E-584B-4FAF-BDA4-EF5A7B90C4C1}</productID>
					<productID>{DEDA7399-29FE-40A5-8B64-F630D40EF8C0}</productID>
					<productID>{EB6C9E35-CBA2-4C2B-8958-55EA6F0EA707}</productID>
					<productID>{EDEDDCAE-21E6-4A82-9B58-8E6337386685}</productID>
					<productID>{EFC648C5-F3BC-4096-9AFE-23121EF06828}</productID>
					<productID>{F2EEE63A-0DCA-41BF-A243-4E4C0DFA38A4}</productID></dependencies></product></baselineProducts>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\data\WarningLights_And_Sirens\WarningLights_And_Sirens.prj" ProjectRelativePath="WarningLights_And_Sirens.prj"/></Projects>
		<buildData progressBarRate="8.952927672272207">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.521299304687494</ProductsAdded>
				<DPConfigured>0.699582934997542</DPConfigured>
				<DPMergeModulesAdded>0.754533684051491</DPMergeModulesAdded>
				<DPClosed>1.109139887695289</DPClosed>
				<DistributionsCopied>1.232972942901575</DistributionsCopied>
				<End>11.169530645233117</End></progressTimes></buildData>
	</msi>
</distribution>
