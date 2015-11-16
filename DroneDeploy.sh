#!/bin/bash

#
# TODO
# 	Single file for deploying
# 	Use argument to select folder
#	Pull variables and service files from folder
# 	Detect package system (pacman, debian)
# 	Detect service system (systemd, upstart)
#
#	Usage: DroneDeploy Flight
#	
#	0. Install/check dependencies (package system)
#	1. Git clone/pull
#	2. Build (folder, package system, service system)
#	3. Install (package system)
#	4. Restart services [Flight, Tower] (service system)
#

###################################################################
# 0. Establish user-ship, sudo, and package manager (package_system)
###################################################################
function end {
	echo "Error: $1"
	exit 1
}
function has_cmd {
	command -v "$1" >/dev/null 2>&1 && return 0 || return 1
}
function has_pkg {
	if [ "$package_manager" == "apt-get" ]; then
		dpkg -l $1 >/dev/null 2>&1
	elif [ "$package_manager" == "pacman" ]; then
		pacman -Qs $1 >/dev/null 2>&1
	else
		end "Called has_pkg $1 without a valid package_manager: $package_manager"
	fi
}
function get_deps {
	
	# Convert generic dependency names to system-specific packages
	pkg_list=""
	for dep in $@; do
		echo "Dependency: $dep"
		if [ "$package_manager" == "apt-get" ]
		then
			[ "$dep" == "git" ]   && pkg_list="$pkg_list git"
			[ "$dep" == "wget" ]  && pkg_list="$pkg_list wget"
			[ "$dep" == "qmake" ] && pkg_list="$pkg_list qtchooser qt5-qmake"

			# None include headers or .pc files unless otherwise stated
			[ "$dep" == "qt5-concurrent" ]   && pkg_list="$pkg_list libqt5concurrent5"
			[ "$dep" == "qt5-core" ]         && pkg_list="$pkg_list libqt5core5a"
			[ "$dep" == "qt5-declarative" ]  && pkg_list="$pkg_list libqt5declarative5"
			[ "$dep" == "qt5-gui" ]          && pkg_list="$pkg_list libqt5gui5"
			[ "$dep" == "qt5-multimedia" ]   && pkg_list="$pkg_list libqt5multimedia5 libqt5multimedia5-plugins libqt5multimediaquick-p5 libqt5multimediawidgets5"
			[ "$dep" == "qt5-network" ]      && pkg_list="$pkg_list libqt5network5"
			[ "$dep" == "qt5-opengl" ]       && pkg_list="$pkg_list libqt5opengl5 libqt5opengl5-dev"
			[ "$dep" == "qt5-printsupport" ] && pkg_list="$pkg_list libqt5printsupport5"
			[ "$dep" == "qt5-qml" ]          && pkg_list="$pkg_list libqt5qml5"
			[ "$dep" == "qt5-quick" ]        && pkg_list="$pkg_list libqt5quick5 libqt5quickparticles5 libqt5quicktest5 libqt5quickwidgets5"
			[ "$dep" == "qt5-serialport" ]   && pkg_list="$pkg_list libqt5serialport5 libqt5serialport5-dev"
			[ "$dep" == "qt5-sql" ]          && pkg_list="$pkg_list libqt5sql5 libqt5sql5-mysql libqt5sql5-sqlite" # 3 More not here
			[ "$dep" == "qt5-widgets" ]      && pkg_list="$pkg_list libqt5widgets5"
			[ "$dep" == "qt5-webkit" ]       && pkg_list="$pkg_list libqt5webkit5 libqt5webkit5-dev libqt5webkit5-dbg"
			[ "$dep" == "qt5-websockets" ]   && pkg_list="$pkg_list libqt5websockets5 libqt5websockets5-dev"

			[ "$dep" == "qt5-concurrent" ]   && pkg_list="$pkg_list qtbase5-dev"
			[ "$dep" == "qt5-core" ]         && pkg_list="$pkg_list qtbase5-dev"
			[ "$dep" == "qt5-declarative" ]  && pkg_list="$pkg_list qtquick1-5-dev"
			[ "$dep" == "qt5-gui" ]          && pkg_list="$pkg_list qtbase5-dev"
			[ "$dep" == "qt5-multimedia" ]   && pkg_list="$pkg_list qtmultimedia5-dev"
			[ "$dep" == "qt5-network" ]      && pkg_list="$pkg_list qtbase5-dev"
			[ "$dep" == "qt5-opengl" ]       && pkg_list="$pkg_list "
			[ "$dep" == "qt5-printsupport" ] && pkg_list="$pkg_list qtbase5-dev"
			[ "$dep" == "qt5-qml" ]          && pkg_list="$pkg_list qtdeclarative5-dev"
			[ "$dep" == "qt5-quick" ]        && pkg_list="$pkg_list qtdeclarative5-dev"
			[ "$dep" == "qt5-serialport" ]   && pkg_list="$pkg_list "
			[ "$dep" == "qt5-sql" ]          && pkg_list="$pkg_list qtbase5-dev"
			[ "$dep" == "qt5-widgets" ]      && pkg_list="$pkg_list qtbase5-dev"
			[ "$dep" == "qt5-webkit" ]       && pkg_list="$pkg_list "
			[ "$dep" == "qt5-websockets" ]   && pkg_list="$pkg_list "

		elif [ "$package_manager" == "pacman" ]
		then
			[ "$dep" == "git" ]   && pkg_list="$pkg_list git"
			[ "$dep" == "wget" ]  && pkg_list="$pkg_list wget"
			[ "$dep" == "qmake" ] && pkg_list="$pkg_list qtchooser qt5-base"

			[ "$dep" == "qt5-concurrent" ]   && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-core" ]         && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-declarative" ]  && pkg_list="$pkg_list qt5-quick1"
			[ "$dep" == "qt5-gui" ]          && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-multimedia" ]   && pkg_list="$pkg_list qt5-multimedia"
			[ "$dep" == "qt5-network" ]      && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-opengl" ]       && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-printsupport" ] && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-qml" ]          && pkg_list="$pkg_list qt5-declarative"
			[ "$dep" == "qt5-quick" ]        && pkg_list="$pkg_list qt5-declarative"
			[ "$dep" == "qt5-serialport" ]   && pkg_list="$pkg_list qt5-serialport"
			[ "$dep" == "qt5-sql" ]          && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-widgets" ]      && pkg_list="$pkg_list qt5-base"
			[ "$dep" == "qt5-webkit" ]       && pkg_list="$pkg_list qt5-webkit"
			[ "$dep" == "qt5-websockets" ]   && pkg_list="$pkg_list qt5-websockets"
		fi
	done

	# Filter out packages that are already installed
	pkg_list_unfiltered="$pkg_list"
	pkg_list=""
	for pkg in $pkg_list_unfiltered; do
		has_pkg "$pkg" || pkg_list="$pkg_list $pkg"
	done
	[ "$pkg_list" == "" ] || [ "$pkg_list" == " " ] && return

	# Install them
	echo "3. Installing packages: $pkg_list"
	if [ "$package_manager" == "apt-get" ]; then
		sudo apt-get install $pkg_list || end "Failed while installing packages: sudo apt-get install $pkg_list"
	elif [ "$package_manager" == "pacman" ]; then
		sudo pacman -S $pkg_list || end "Failed while installing packages: sudo pacman -S $pkg_list"
	else
		end "Called get_pkg without a valid package_manager set: $package_manager"
	fi
}
# Must run as a regular user using 
# TODO Check if this is a non-root user
has_cmd "sudo" || end "Please install sudo"

# Detect package manager
has_cmd "apt-get" && package_manager="apt-get"
has_cmd "pacman" && package_manager="pacman"
[ "$package_manager" ] || end "Unsupported package manager"
echo "Package Manager: $package_manager"

#####################################################
# 1. Clone/update repo, get version (rundir, version)
#####################################################
rundir=$(pwd)
giturl="https://github.com/dustyco/Drone"
gitname="Drone"

function clone_repo {
	has_cmd "git" || get_deps "git" || end "Git not installed and could not be installed"
	[ -d "$gitname" ] && cd "$gitname" && git pull \
	|| git clone "$giturl" \
	|| end "Failed to clone from git"

	cd "$rundir/$gitname"
	version=$(git describe --always --tags --long | sed 's|-|.|g')
	cd "$rundir"
	echo "Version: $version"
}

# Clone a copy of the repo
clone_repo || end "Failed to clone git repo"

##################################################
# 2. Install dependencies (subfolder, deployfiles)
##################################################
has_cmd "qmake" || get_deps "qmake" || end "Qmake not installed and could not be installed"
subfolder="$1"
[ "$subfolder" ] || end "Usage: $0 [subfolder]"
[ -d "$gitname/$subfolder" ] || end "No such subfolder: $gitname/$subfolder"
deployfiles="$rundir/$gitname/$subfolder/DeployFiles"
[ -d "$deployfiles" ] || end "Missing folder: $deployfiles"
[ -f "$deployfiles/Dependencies.txt" ] && get_deps $(cat "$deployfiles/Dependencies.txt")

#################################
# 2. Build and optionally install
#################################
# Ubuntu: apt-get
# Arch: pacman
# AWS: yum/rpm (rehat package manager)
# SUSE: zypper

function lowercase {
	echo $@ | tr '[:upper:]' '[:lower:]'
}

# TODO This does not work if you type "n"
# Ask if we want to install the package too
echo "Subfolder: $subfolder"
echo "Starting build"
printf "Install after? [Y/n]: "; read install_response
[ "$response" == "n" ] || do_install="True"

if [ "$package_manager" == "apt-get" ]; then

	      pkgname="drone-$(lowercase $subfolder)-git"
	      pkgdesc="Drone $subfolder runtime"
	   maintainer="Dustin Story <dustin.story@gmail.com>"
	build_depends="debhelper (>= 9)"
	  run_depends="${shlibs:Depends}, ${misc:Depends}"
	       pkgdir="$pkgname-$version"

	# Remove existing package source folder and make a new copy from git repo
	[ -d "$pkgdir" ] && echo "Removing existing source folder" && rm -rf "$pkgdir"
	[ -d "$pkgdir" ] && end "Failed to remove existing source folder"
	cp -r "$gitname" "$pkgdir" && [ -d "$pkgdir" ] || end "Failed to make a working copy of the git repo"

	# Start adding debian specific configuration files
	debdir="$pkgdir/debian"
	mkdir "$debdir" && [ -d "$debdir" ] || end "Failed to create $debdir"

	# Debian: Changelog
	echo "$pkgname ($version) UNRELEASED; urgency=medium"  > "$debdir/changelog"
	echo ""                                                 >> "$debdir/changelog"
	echo "  * Initial release. (Closes: #XXXXXX)"           >> "$debdir/changelog"
	echo ""                                                 >> "$debdir/changelog"
	echo " -- $maintainer  Fri, 13 Nov 2015 07:38:30 +0000" >> "$debdir/changelog" # TODO use date command

	# Debian: Compatibility level
	echo 9                                > "$debdir/compat"

	# Debian: General info
	echo "Source: $pkgname"               > "$debdir/control"
	echo "Maintainer: $maintainer"       >> "$debdir/control"
	echo "Section: misc"                 >> "$debdir/control"
	echo "Priority: optional"            >> "$debdir/control"
	echo "Standards-Version: 3.9.2"      >> "$debdir/control"
	echo "Build-Depends: $build_depends" >> "$debdir/control"
	echo ""                              >> "$debdir/control"
	echo "Package: $pkgname"             >> "$debdir/control"
	echo "Architecture: any"             >> "$debdir/control"
	echo "Depends: $run_depends"         >> "$debdir/control"
	echo "Description: $pkgdesc"         >> "$debdir/control"


	# Debian: Copyright
#	echo ""                               > "$debdir/copyright"

	# Debian: General info
	echo "#!/usr/bin/make -f"                      > "$debdir/rules"
	echo "%:"                                     >> "$debdir/rules"
	echo "	dh \$@"                               >> "$debdir/rules"
	echo ""                                       >> "$debdir/rules"
	echo "override_dh_auto_configure:"            >> "$debdir/rules"
	echo "	qmake -config release \"$subfolder\"" >> "$debdir/rules"
	echo ""                                       >> "$debdir/rules"
	echo "override_dh_auto_build:"                >> "$debdir/rules"
	echo "	make"                                 >> "$debdir/rules"
	echo ""                                       >> "$debdir/rules"
	echo "override_dh_auto_install:"              >> "$debdir/rules"
	echo "	\"$subfolder/DeployFiles/Install.sh\" \".\"  \".\" \"debian/$pkgname\""     >> "$debdir/rules"


  	echo "================= Building debian package =================="
  	cd "$pkgdir"
  	debuild -us -uc || end "Failed to build package"
  	cd "$rundir"

	echo "=============== Installing debian package =================="
	if [ "$do_install" ]; then
  		cd "$rundir"
		sudo dpkg -i "$pkgname_$version_"*".deb" || end "Failed to install package"

		echo "Cleaning up build files"
		rm -rf "$pkgname_$version"* || end "Failed to cleanup build files"
	fi

elif [ "$package_manager" == "pacman" ]; then

	# rsync -r . max:Drone && ssh -t max "Drone/DroneDeploy.sh Tower"
	# http://askubuntu.com/questions/281742/sudo-no-tty-present-and-no-askpass-program-specified
	get_deps wget
	wget -N "https://raw.githubusercontent.com/dustyco/Drone/master/$subfolder/DeployFiles/pacman/PKGBUILD" || end "Could not download PKGBUILD"

	[ "$do_install" ] && installarg="--install"
	makepkg -cf --syncdeps "$installarg" --cleanbuild || end "makepkg failed"
else
	end "No build function defined for this package_manager: $package_manager"
fi

######################################################
# 4. Restart services [Flight, Tower] (service system)
######################################################

# TODO
# sudo service drone-tower start
# sudo systemctl start DroneTower




