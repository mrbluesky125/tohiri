#
# TOHIRI spec file
# (C) kimmoli 2014-2015
# 

Name:       harbour-ambience-tohiri

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    The OtherHalf Thermal Imager
Version:    0.6.0
Release:    5
Group:      Qt/Qt
License:    LICENSE
URL:        https://github.com/kimmoli/tohiri
Source0:    %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 0.0.10
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Multimedia)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  desktop-file-utils

Requires:   ambienced

Obsoletes:  harbour-tohiri

%description
Thermal Imaging application to be used with TOHIRI OtherHalf


%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5 SPECVERSION=%{version}
%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}
%{_datadir}/icons/hicolor/86x86/apps/
%{_datadir}/applications/
%{_datadir}/harbour-tohiri/
%{_datadir}/harbour-tohiri/qml
%{_datadir}/ambience/%{name}/
%{_datadir}/ambience/%{name}/images/

%post

%_ambience_post
