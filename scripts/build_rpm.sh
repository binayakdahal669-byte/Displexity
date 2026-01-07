#!/bin/bash
# Displexity RPM Package Builder
# Creates .rpm package for Fedora/RHEL/CentOS

set -e

VERSION="${1:-1.2.0}"
RELEASE="${2:-1}"

echo "Creating Displexity RPM package v${VERSION}..."

# Check for rpmbuild
if ! command -v rpmbuild &> /dev/null; then
    echo "Error: rpmbuild not found"
    echo "Install with: sudo dnf install rpm-build (Fedora)"
    echo "           or: sudo yum install rpm-build (RHEL/CentOS)"
    exit 1
fi

# Set up RPM build directories
RPMBUILD_DIR="$HOME/rpmbuild"
mkdir -p "$RPMBUILD_DIR"/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

# Create source tarball
SOURCE_DIR="displexity-${VERSION}"
mkdir -p "$SOURCE_DIR"

# Copy files to source directory
cp -r bin "$SOURCE_DIR/" 2>/dev/null || cp -r release/bin "$SOURCE_DIR/bin" 2>/dev/null || true
cp -r release/lib "$SOURCE_DIR/lib" 2>/dev/null || mkdir -p "$SOURCE_DIR/lib"
cp -r release/include "$SOURCE_DIR/include" 2>/dev/null || mkdir -p "$SOURCE_DIR/include"
cp -r release/docs "$SOURCE_DIR/docs" 2>/dev/null || mkdir -p "$SOURCE_DIR/docs"
cp -r release/examples "$SOURCE_DIR/examples" 2>/dev/null || mkdir -p "$SOURCE_DIR/examples"

# Create tarball
tar -czf "$RPMBUILD_DIR/SOURCES/displexity-${VERSION}.tar.gz" "$SOURCE_DIR"
rm -rf "$SOURCE_DIR"

# Create spec file
cat > "$RPMBUILD_DIR/SPECS/displexity.spec" << EOF
Name:           displexity
Version:        ${VERSION}
Release:        ${RELEASE}%{?dist}
Summary:        Displexity Programming Language Compiler

License:        MIT
URL:            https://github.com/displexity
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc
Requires:       glibc

%description
Displexity is a modern programming language designed for
game development, system programming, and cross-platform
applications. Features include simple syntax, TUI bytecode
for portable terminal apps, and cross-platform compilation.

%prep
%setup -q

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_libdir}/displexity
mkdir -p %{buildroot}%{_includedir}/displexity
mkdir -p %{buildroot}%{_docdir}/%{name}
mkdir -p %{buildroot}%{_datadir}/%{name}/examples

# Install binaries
install -m 755 bin/disp %{buildroot}%{_bindir}/disp 2>/dev/null || true
ln -sf disp %{buildroot}%{_bindir}/dis
ln -sf disp %{buildroot}%{_bindir}/dih
ln -sf disp %{buildroot}%{_bindir}/di

# Install libraries
cp lib/displexity/*.disll %{buildroot}%{_libdir}/displexity/ 2>/dev/null || true

# Install headers
cp include/displexity/*.dish %{buildroot}%{_includedir}/displexity/ 2>/dev/null || true
cp include/displexity/*.tuy %{buildroot}%{_includedir}/displexity/ 2>/dev/null || true

# Install docs
cp docs/*.md %{buildroot}%{_docdir}/%{name}/ 2>/dev/null || true

# Install examples
cp examples/*.dis %{buildroot}%{_datadir}/%{name}/examples/ 2>/dev/null || true

%files
%license LICENSE
%doc README.md
%{_bindir}/disp
%{_bindir}/dis
%{_bindir}/dih
%{_bindir}/di
%{_libdir}/displexity/
%{_includedir}/displexity/
%{_docdir}/%{name}/
%{_datadir}/%{name}/

%changelog
* $(date "+%a %b %d %Y") Displexity Team <team@displexity.dev> - ${VERSION}-${RELEASE}
- Initial RPM release
- TUI bytecode support
- Cross-platform compilation
EOF

# Build the RPM
rpmbuild -ba "$RPMBUILD_DIR/SPECS/displexity.spec"

# Copy result to current directory
cp "$RPMBUILD_DIR/RPMS/"*"/displexity-${VERSION}-${RELEASE}"*.rpm . 2>/dev/null || true

echo "RPM package created!"
echo "Install with: sudo rpm -i displexity-${VERSION}-${RELEASE}.*.rpm"
echo "Or with dnf: sudo dnf install displexity-${VERSION}-${RELEASE}.*.rpm"
