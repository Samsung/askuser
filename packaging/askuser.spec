Name:       askuser
Summary:    Agent service for Cynara 'ask user' policy
Version:    0.0.1
Release:    1
Group:      Security/Access Control
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    %{name}.manifest
Source1002:    libaskuser-common.manifest
Source1003:    askuser-plugins.manifest
BuildRequires: cmake
BuildRequires: pkgconfig(cynara-agent)
BuildRequires: pkgconfig(cynara-plugin)
BuildRequires: pkgconfig(libsystemd-daemon)
BuildRequires: pkgconfig(libsystemd-journal)
BuildRequires: zip
%{?systemd_requires}

%if !%{defined build_type}
%define build_type RELEASE
%endif

%description
Daemon allowing user to grant or deny acces for given application and privilege

%package -n libaskuser-common
Summary:    Askuser common library

%description -n libaskuser-common
askuser common library with common functionalities

%package -n askuser-plugins
Requires:   cynara
Requires:   libcynara-client
Summary:    Askuser commons library

%description -n askuser-plugins
askuser plugin library with cynara service and client side plugins

%prep
%setup -q
cp -a %{SOURCE1001} .
cp -a %{SOURCE1002} .
cp -a %{SOURCE1003} .

%build
%if 0%{?sec_build_binary_debug_enable}
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if %{?build_type} == "DEBUG"
export CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post
systemctl daemon-reload

if [ $1 = 1 ]; then
    systemctl enable %{name}.service
fi

systemctl restart %{name}.service

%preun
if [ $1 = 0 ]; then
    systemctl stop %{name}.service
fi

%postun
if [ $1 = 0 ]; then
    systemctl daemon-reload
fi

%post -n libaskuser-common -p /sbin/ldconfig

%postun -n libaskuser-common -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%license LICENSE
%attr(755,root,root) /usr/bin/%{name}
/usr/lib/systemd/system/%{name}.service

%files -n libaskuser-common
%manifest libaskuser-common.manifest
%license LICENSE
%{_libdir}/libaskuser-common.so*

%files -n askuser-plugins
%manifest askuser-plugins.manifest
%license LICENSE
%{_libdir}/cynara/plugin/client/*
%{_libdir}/cynara/plugin/service/*
