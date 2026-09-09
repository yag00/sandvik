/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

// Generated table: android.system.OsConstants field name -> real host value.
// OsConstants.<clinit> calls placeholder() once per field, immediately followed by
// an sput to that field; this table is consumed in field-declaration order.
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>

#include <cstdint>
#include <string>
#include <vector>

#if __has_include(<linux/capability.h>)
#include <linux/capability.h>
#endif
#if __has_include(<linux/vm_sockets.h>)
#include <linux/vm_sockets.h>
#endif

namespace sandvik {
	namespace osconst {
		/** Represents an entry in the OS constants table. */
		struct Entry {
				/** The name of the OS constant. */
				const char* name;
				/** The value of the OS constant. */
				long value;
		};

		const std::vector<Entry>& orderedTable() {
			static const std::vector<Entry> table = {
#ifdef AF_INET
			    {"AF_INET", static_cast<long>(AF_INET)},
#else
			    {"AF_INET", -1},
#endif
#ifdef AF_INET6
			    {"AF_INET6", static_cast<long>(AF_INET6)},
#else
			    {"AF_INET6", -1},
#endif
#ifdef AF_NETLINK
			    {"AF_NETLINK", static_cast<long>(AF_NETLINK)},
#else
			    {"AF_NETLINK", -1},
#endif
#ifdef AF_PACKET
			    {"AF_PACKET", static_cast<long>(AF_PACKET)},
#else
			    {"AF_PACKET", -1},
#endif
#ifdef AF_UNIX
			    {"AF_UNIX", static_cast<long>(AF_UNIX)},
#else
			    {"AF_UNIX", -1},
#endif
#ifdef AF_VSOCK
			    {"AF_VSOCK", static_cast<long>(AF_VSOCK)},
#else
			    {"AF_VSOCK", -1},
#endif
#ifdef AF_UNSPEC
			    {"AF_UNSPEC", static_cast<long>(AF_UNSPEC)},
#else
			    {"AF_UNSPEC", -1},
#endif
#ifdef AI_ADDRCONFIG
			    {"AI_ADDRCONFIG", static_cast<long>(AI_ADDRCONFIG)},
#else
			    {"AI_ADDRCONFIG", -1},
#endif
#ifdef AI_ALL
			    {"AI_ALL", static_cast<long>(AI_ALL)},
#else
			    {"AI_ALL", -1},
#endif
#ifdef AI_CANONNAME
			    {"AI_CANONNAME", static_cast<long>(AI_CANONNAME)},
#else
			    {"AI_CANONNAME", -1},
#endif
#ifdef AI_NUMERICHOST
			    {"AI_NUMERICHOST", static_cast<long>(AI_NUMERICHOST)},
#else
			    {"AI_NUMERICHOST", -1},
#endif
#ifdef AI_NUMERICSERV
			    {"AI_NUMERICSERV", static_cast<long>(AI_NUMERICSERV)},
#else
			    {"AI_NUMERICSERV", -1},
#endif
#ifdef AI_PASSIVE
			    {"AI_PASSIVE", static_cast<long>(AI_PASSIVE)},
#else
			    {"AI_PASSIVE", -1},
#endif
#ifdef AI_V4MAPPED
			    {"AI_V4MAPPED", static_cast<long>(AI_V4MAPPED)},
#else
			    {"AI_V4MAPPED", -1},
#endif
#ifdef ARPHRD_ETHER
			    {"ARPHRD_ETHER", static_cast<long>(ARPHRD_ETHER)},
#else
			    {"ARPHRD_ETHER", -1},
#endif
#ifdef VMADDR_PORT_ANY
			    {"VMADDR_PORT_ANY", static_cast<long>(VMADDR_PORT_ANY)},
#else
			    {"VMADDR_PORT_ANY", -1},
#endif
#ifdef VMADDR_CID_ANY
			    {"VMADDR_CID_ANY", static_cast<long>(VMADDR_CID_ANY)},
#else
			    {"VMADDR_CID_ANY", -1},
#endif
#ifdef VMADDR_CID_LOCAL
			    {"VMADDR_CID_LOCAL", static_cast<long>(VMADDR_CID_LOCAL)},
#else
			    {"VMADDR_CID_LOCAL", -1},
#endif
#ifdef VMADDR_CID_HOST
			    {"VMADDR_CID_HOST", static_cast<long>(VMADDR_CID_HOST)},
#else
			    {"VMADDR_CID_HOST", -1},
#endif
#ifdef ARPHRD_LOOPBACK
			    {"ARPHRD_LOOPBACK", static_cast<long>(ARPHRD_LOOPBACK)},
#else
			    {"ARPHRD_LOOPBACK", -1},
#endif
#ifdef CAP_AUDIT_CONTROL
			    {"CAP_AUDIT_CONTROL", static_cast<long>(CAP_AUDIT_CONTROL)},
#else
			    {"CAP_AUDIT_CONTROL", -1},
#endif
#ifdef CAP_AUDIT_WRITE
			    {"CAP_AUDIT_WRITE", static_cast<long>(CAP_AUDIT_WRITE)},
#else
			    {"CAP_AUDIT_WRITE", -1},
#endif
#ifdef CAP_BLOCK_SUSPEND
			    {"CAP_BLOCK_SUSPEND", static_cast<long>(CAP_BLOCK_SUSPEND)},
#else
			    {"CAP_BLOCK_SUSPEND", -1},
#endif
#ifdef CAP_CHOWN
			    {"CAP_CHOWN", static_cast<long>(CAP_CHOWN)},
#else
			    {"CAP_CHOWN", -1},
#endif
#ifdef CAP_DAC_OVERRIDE
			    {"CAP_DAC_OVERRIDE", static_cast<long>(CAP_DAC_OVERRIDE)},
#else
			    {"CAP_DAC_OVERRIDE", -1},
#endif
#ifdef CAP_DAC_READ_SEARCH
			    {"CAP_DAC_READ_SEARCH", static_cast<long>(CAP_DAC_READ_SEARCH)},
#else
			    {"CAP_DAC_READ_SEARCH", -1},
#endif
#ifdef CAP_FOWNER
			    {"CAP_FOWNER", static_cast<long>(CAP_FOWNER)},
#else
			    {"CAP_FOWNER", -1},
#endif
#ifdef CAP_FSETID
			    {"CAP_FSETID", static_cast<long>(CAP_FSETID)},
#else
			    {"CAP_FSETID", -1},
#endif
#ifdef CAP_IPC_LOCK
			    {"CAP_IPC_LOCK", static_cast<long>(CAP_IPC_LOCK)},
#else
			    {"CAP_IPC_LOCK", -1},
#endif
#ifdef CAP_IPC_OWNER
			    {"CAP_IPC_OWNER", static_cast<long>(CAP_IPC_OWNER)},
#else
			    {"CAP_IPC_OWNER", -1},
#endif
#ifdef CAP_KILL
			    {"CAP_KILL", static_cast<long>(CAP_KILL)},
#else
			    {"CAP_KILL", -1},
#endif
#ifdef CAP_LAST_CAP
			    {"CAP_LAST_CAP", static_cast<long>(CAP_LAST_CAP)},
#else
			    {"CAP_LAST_CAP", -1},
#endif
#ifdef CAP_LEASE
			    {"CAP_LEASE", static_cast<long>(CAP_LEASE)},
#else
			    {"CAP_LEASE", -1},
#endif
#ifdef CAP_LINUX_IMMUTABLE
			    {"CAP_LINUX_IMMUTABLE", static_cast<long>(CAP_LINUX_IMMUTABLE)},
#else
			    {"CAP_LINUX_IMMUTABLE", -1},
#endif
#ifdef CAP_MAC_ADMIN
			    {"CAP_MAC_ADMIN", static_cast<long>(CAP_MAC_ADMIN)},
#else
			    {"CAP_MAC_ADMIN", -1},
#endif
#ifdef CAP_MAC_OVERRIDE
			    {"CAP_MAC_OVERRIDE", static_cast<long>(CAP_MAC_OVERRIDE)},
#else
			    {"CAP_MAC_OVERRIDE", -1},
#endif
#ifdef CAP_MKNOD
			    {"CAP_MKNOD", static_cast<long>(CAP_MKNOD)},
#else
			    {"CAP_MKNOD", -1},
#endif
#ifdef CAP_NET_ADMIN
			    {"CAP_NET_ADMIN", static_cast<long>(CAP_NET_ADMIN)},
#else
			    {"CAP_NET_ADMIN", -1},
#endif
#ifdef CAP_NET_BIND_SERVICE
			    {"CAP_NET_BIND_SERVICE", static_cast<long>(CAP_NET_BIND_SERVICE)},
#else
			    {"CAP_NET_BIND_SERVICE", -1},
#endif
#ifdef CAP_NET_BROADCAST
			    {"CAP_NET_BROADCAST", static_cast<long>(CAP_NET_BROADCAST)},
#else
			    {"CAP_NET_BROADCAST", -1},
#endif
#ifdef CAP_NET_RAW
			    {"CAP_NET_RAW", static_cast<long>(CAP_NET_RAW)},
#else
			    {"CAP_NET_RAW", -1},
#endif
#ifdef CAP_SETFCAP
			    {"CAP_SETFCAP", static_cast<long>(CAP_SETFCAP)},
#else
			    {"CAP_SETFCAP", -1},
#endif
#ifdef CAP_SETGID
			    {"CAP_SETGID", static_cast<long>(CAP_SETGID)},
#else
			    {"CAP_SETGID", -1},
#endif
#ifdef CAP_SETPCAP
			    {"CAP_SETPCAP", static_cast<long>(CAP_SETPCAP)},
#else
			    {"CAP_SETPCAP", -1},
#endif
#ifdef CAP_SETUID
			    {"CAP_SETUID", static_cast<long>(CAP_SETUID)},
#else
			    {"CAP_SETUID", -1},
#endif
#ifdef CAP_SYS_ADMIN
			    {"CAP_SYS_ADMIN", static_cast<long>(CAP_SYS_ADMIN)},
#else
			    {"CAP_SYS_ADMIN", -1},
#endif
#ifdef CAP_SYS_BOOT
			    {"CAP_SYS_BOOT", static_cast<long>(CAP_SYS_BOOT)},
#else
			    {"CAP_SYS_BOOT", -1},
#endif
#ifdef CAP_SYS_CHROOT
			    {"CAP_SYS_CHROOT", static_cast<long>(CAP_SYS_CHROOT)},
#else
			    {"CAP_SYS_CHROOT", -1},
#endif
#ifdef CAP_SYSLOG
			    {"CAP_SYSLOG", static_cast<long>(CAP_SYSLOG)},
#else
			    {"CAP_SYSLOG", -1},
#endif
#ifdef CAP_SYS_MODULE
			    {"CAP_SYS_MODULE", static_cast<long>(CAP_SYS_MODULE)},
#else
			    {"CAP_SYS_MODULE", -1},
#endif
#ifdef CAP_SYS_NICE
			    {"CAP_SYS_NICE", static_cast<long>(CAP_SYS_NICE)},
#else
			    {"CAP_SYS_NICE", -1},
#endif
#ifdef CAP_SYS_PACCT
			    {"CAP_SYS_PACCT", static_cast<long>(CAP_SYS_PACCT)},
#else
			    {"CAP_SYS_PACCT", -1},
#endif
#ifdef CAP_SYS_PTRACE
			    {"CAP_SYS_PTRACE", static_cast<long>(CAP_SYS_PTRACE)},
#else
			    {"CAP_SYS_PTRACE", -1},
#endif
#ifdef CAP_SYS_RAWIO
			    {"CAP_SYS_RAWIO", static_cast<long>(CAP_SYS_RAWIO)},
#else
			    {"CAP_SYS_RAWIO", -1},
#endif
#ifdef CAP_SYS_RESOURCE
			    {"CAP_SYS_RESOURCE", static_cast<long>(CAP_SYS_RESOURCE)},
#else
			    {"CAP_SYS_RESOURCE", -1},
#endif
#ifdef CAP_SYS_TIME
			    {"CAP_SYS_TIME", static_cast<long>(CAP_SYS_TIME)},
#else
			    {"CAP_SYS_TIME", -1},
#endif
#ifdef CAP_SYS_TTY_CONFIG
			    {"CAP_SYS_TTY_CONFIG", static_cast<long>(CAP_SYS_TTY_CONFIG)},
#else
			    {"CAP_SYS_TTY_CONFIG", -1},
#endif
#ifdef CAP_WAKE_ALARM
			    {"CAP_WAKE_ALARM", static_cast<long>(CAP_WAKE_ALARM)},
#else
			    {"CAP_WAKE_ALARM", -1},
#endif
#ifdef E2BIG
			    {"E2BIG", static_cast<long>(E2BIG)},
#else
			    {"E2BIG", -1},
#endif
#ifdef EACCES
			    {"EACCES", static_cast<long>(EACCES)},
#else
			    {"EACCES", -1},
#endif
#ifdef EADDRINUSE
			    {"EADDRINUSE", static_cast<long>(EADDRINUSE)},
#else
			    {"EADDRINUSE", -1},
#endif
#ifdef EADDRNOTAVAIL
			    {"EADDRNOTAVAIL", static_cast<long>(EADDRNOTAVAIL)},
#else
			    {"EADDRNOTAVAIL", -1},
#endif
#ifdef EAFNOSUPPORT
			    {"EAFNOSUPPORT", static_cast<long>(EAFNOSUPPORT)},
#else
			    {"EAFNOSUPPORT", -1},
#endif
#ifdef EAGAIN
			    {"EAGAIN", static_cast<long>(EAGAIN)},
#else
			    {"EAGAIN", -1},
#endif
#ifdef EAI_AGAIN
			    {"EAI_AGAIN", static_cast<long>(EAI_AGAIN)},
#else
			    {"EAI_AGAIN", -1},
#endif
#ifdef EAI_BADFLAGS
			    {"EAI_BADFLAGS", static_cast<long>(EAI_BADFLAGS)},
#else
			    {"EAI_BADFLAGS", -1},
#endif
#ifdef EAI_FAIL
			    {"EAI_FAIL", static_cast<long>(EAI_FAIL)},
#else
			    {"EAI_FAIL", -1},
#endif
#ifdef EAI_FAMILY
			    {"EAI_FAMILY", static_cast<long>(EAI_FAMILY)},
#else
			    {"EAI_FAMILY", -1},
#endif
#ifdef EAI_MEMORY
			    {"EAI_MEMORY", static_cast<long>(EAI_MEMORY)},
#else
			    {"EAI_MEMORY", -1},
#endif
#ifdef EAI_NODATA
			    {"EAI_NODATA", static_cast<long>(EAI_NODATA)},
#else
			    {"EAI_NODATA", -1},
#endif
#ifdef EAI_NONAME
			    {"EAI_NONAME", static_cast<long>(EAI_NONAME)},
#else
			    {"EAI_NONAME", -1},
#endif
#ifdef EAI_OVERFLOW
			    {"EAI_OVERFLOW", static_cast<long>(EAI_OVERFLOW)},
#else
			    {"EAI_OVERFLOW", -1},
#endif
#ifdef EAI_SERVICE
			    {"EAI_SERVICE", static_cast<long>(EAI_SERVICE)},
#else
			    {"EAI_SERVICE", -1},
#endif
#ifdef EAI_SOCKTYPE
			    {"EAI_SOCKTYPE", static_cast<long>(EAI_SOCKTYPE)},
#else
			    {"EAI_SOCKTYPE", -1},
#endif
#ifdef EAI_SYSTEM
			    {"EAI_SYSTEM", static_cast<long>(EAI_SYSTEM)},
#else
			    {"EAI_SYSTEM", -1},
#endif
#ifdef EALREADY
			    {"EALREADY", static_cast<long>(EALREADY)},
#else
			    {"EALREADY", -1},
#endif
#ifdef EBADF
			    {"EBADF", static_cast<long>(EBADF)},
#else
			    {"EBADF", -1},
#endif
#ifdef EBADMSG
			    {"EBADMSG", static_cast<long>(EBADMSG)},
#else
			    {"EBADMSG", -1},
#endif
#ifdef EBUSY
			    {"EBUSY", static_cast<long>(EBUSY)},
#else
			    {"EBUSY", -1},
#endif
#ifdef ECANCELED
			    {"ECANCELED", static_cast<long>(ECANCELED)},
#else
			    {"ECANCELED", -1},
#endif
#ifdef ECHILD
			    {"ECHILD", static_cast<long>(ECHILD)},
#else
			    {"ECHILD", -1},
#endif
#ifdef ECONNABORTED
			    {"ECONNABORTED", static_cast<long>(ECONNABORTED)},
#else
			    {"ECONNABORTED", -1},
#endif
#ifdef ECONNREFUSED
			    {"ECONNREFUSED", static_cast<long>(ECONNREFUSED)},
#else
			    {"ECONNREFUSED", -1},
#endif
#ifdef ECONNRESET
			    {"ECONNRESET", static_cast<long>(ECONNRESET)},
#else
			    {"ECONNRESET", -1},
#endif
#ifdef EDEADLK
			    {"EDEADLK", static_cast<long>(EDEADLK)},
#else
			    {"EDEADLK", -1},
#endif
#ifdef EDESTADDRREQ
			    {"EDESTADDRREQ", static_cast<long>(EDESTADDRREQ)},
#else
			    {"EDESTADDRREQ", -1},
#endif
#ifdef EDOM
			    {"EDOM", static_cast<long>(EDOM)},
#else
			    {"EDOM", -1},
#endif
#ifdef EDQUOT
			    {"EDQUOT", static_cast<long>(EDQUOT)},
#else
			    {"EDQUOT", -1},
#endif
#ifdef EEXIST
			    {"EEXIST", static_cast<long>(EEXIST)},
#else
			    {"EEXIST", -1},
#endif
#ifdef EFAULT
			    {"EFAULT", static_cast<long>(EFAULT)},
#else
			    {"EFAULT", -1},
#endif
#ifdef EFBIG
			    {"EFBIG", static_cast<long>(EFBIG)},
#else
			    {"EFBIG", -1},
#endif
#ifdef EHOSTUNREACH
			    {"EHOSTUNREACH", static_cast<long>(EHOSTUNREACH)},
#else
			    {"EHOSTUNREACH", -1},
#endif
#ifdef EIDRM
			    {"EIDRM", static_cast<long>(EIDRM)},
#else
			    {"EIDRM", -1},
#endif
#ifdef EILSEQ
			    {"EILSEQ", static_cast<long>(EILSEQ)},
#else
			    {"EILSEQ", -1},
#endif
#ifdef EINPROGRESS
			    {"EINPROGRESS", static_cast<long>(EINPROGRESS)},
#else
			    {"EINPROGRESS", -1},
#endif
#ifdef EINTR
			    {"EINTR", static_cast<long>(EINTR)},
#else
			    {"EINTR", -1},
#endif
#ifdef EINVAL
			    {"EINVAL", static_cast<long>(EINVAL)},
#else
			    {"EINVAL", -1},
#endif
#ifdef EIO
			    {"EIO", static_cast<long>(EIO)},
#else
			    {"EIO", -1},
#endif
#ifdef EISCONN
			    {"EISCONN", static_cast<long>(EISCONN)},
#else
			    {"EISCONN", -1},
#endif
#ifdef EISDIR
			    {"EISDIR", static_cast<long>(EISDIR)},
#else
			    {"EISDIR", -1},
#endif
#ifdef ELOOP
			    {"ELOOP", static_cast<long>(ELOOP)},
#else
			    {"ELOOP", -1},
#endif
#ifdef EMFILE
			    {"EMFILE", static_cast<long>(EMFILE)},
#else
			    {"EMFILE", -1},
#endif
#ifdef EMLINK
			    {"EMLINK", static_cast<long>(EMLINK)},
#else
			    {"EMLINK", -1},
#endif
#ifdef EMSGSIZE
			    {"EMSGSIZE", static_cast<long>(EMSGSIZE)},
#else
			    {"EMSGSIZE", -1},
#endif
#ifdef EMULTIHOP
			    {"EMULTIHOP", static_cast<long>(EMULTIHOP)},
#else
			    {"EMULTIHOP", -1},
#endif
#ifdef ENAMETOOLONG
			    {"ENAMETOOLONG", static_cast<long>(ENAMETOOLONG)},
#else
			    {"ENAMETOOLONG", -1},
#endif
#ifdef ENETDOWN
			    {"ENETDOWN", static_cast<long>(ENETDOWN)},
#else
			    {"ENETDOWN", -1},
#endif
#ifdef ENETRESET
			    {"ENETRESET", static_cast<long>(ENETRESET)},
#else
			    {"ENETRESET", -1},
#endif
#ifdef ENETUNREACH
			    {"ENETUNREACH", static_cast<long>(ENETUNREACH)},
#else
			    {"ENETUNREACH", -1},
#endif
#ifdef ENFILE
			    {"ENFILE", static_cast<long>(ENFILE)},
#else
			    {"ENFILE", -1},
#endif
#ifdef ENOBUFS
			    {"ENOBUFS", static_cast<long>(ENOBUFS)},
#else
			    {"ENOBUFS", -1},
#endif
#ifdef ENODATA
			    {"ENODATA", static_cast<long>(ENODATA)},
#else
			    {"ENODATA", -1},
#endif
#ifdef ENODEV
			    {"ENODEV", static_cast<long>(ENODEV)},
#else
			    {"ENODEV", -1},
#endif
#ifdef ENOENT
			    {"ENOENT", static_cast<long>(ENOENT)},
#else
			    {"ENOENT", -1},
#endif
#ifdef ENOEXEC
			    {"ENOEXEC", static_cast<long>(ENOEXEC)},
#else
			    {"ENOEXEC", -1},
#endif
#ifdef ENOLCK
			    {"ENOLCK", static_cast<long>(ENOLCK)},
#else
			    {"ENOLCK", -1},
#endif
#ifdef ENOLINK
			    {"ENOLINK", static_cast<long>(ENOLINK)},
#else
			    {"ENOLINK", -1},
#endif
#ifdef ENOMEM
			    {"ENOMEM", static_cast<long>(ENOMEM)},
#else
			    {"ENOMEM", -1},
#endif
#ifdef ENOMSG
			    {"ENOMSG", static_cast<long>(ENOMSG)},
#else
			    {"ENOMSG", -1},
#endif
#ifdef ENONET
			    {"ENONET", static_cast<long>(ENONET)},
#else
			    {"ENONET", -1},
#endif
#ifdef ENOPROTOOPT
			    {"ENOPROTOOPT", static_cast<long>(ENOPROTOOPT)},
#else
			    {"ENOPROTOOPT", -1},
#endif
#ifdef ENOSPC
			    {"ENOSPC", static_cast<long>(ENOSPC)},
#else
			    {"ENOSPC", -1},
#endif
#ifdef ENOSR
			    {"ENOSR", static_cast<long>(ENOSR)},
#else
			    {"ENOSR", -1},
#endif
#ifdef ENOSTR
			    {"ENOSTR", static_cast<long>(ENOSTR)},
#else
			    {"ENOSTR", -1},
#endif
#ifdef ENOSYS
			    {"ENOSYS", static_cast<long>(ENOSYS)},
#else
			    {"ENOSYS", -1},
#endif
#ifdef ENOTCONN
			    {"ENOTCONN", static_cast<long>(ENOTCONN)},
#else
			    {"ENOTCONN", -1},
#endif
#ifdef ENOTDIR
			    {"ENOTDIR", static_cast<long>(ENOTDIR)},
#else
			    {"ENOTDIR", -1},
#endif
#ifdef ENOTEMPTY
			    {"ENOTEMPTY", static_cast<long>(ENOTEMPTY)},
#else
			    {"ENOTEMPTY", -1},
#endif
#ifdef ENOTSOCK
			    {"ENOTSOCK", static_cast<long>(ENOTSOCK)},
#else
			    {"ENOTSOCK", -1},
#endif
#ifdef ENOTSUP
			    {"ENOTSUP", static_cast<long>(ENOTSUP)},
#else
			    {"ENOTSUP", -1},
#endif
#ifdef ENOTTY
			    {"ENOTTY", static_cast<long>(ENOTTY)},
#else
			    {"ENOTTY", -1},
#endif
#ifdef ENXIO
			    {"ENXIO", static_cast<long>(ENXIO)},
#else
			    {"ENXIO", -1},
#endif
#ifdef EOPNOTSUPP
			    {"EOPNOTSUPP", static_cast<long>(EOPNOTSUPP)},
#else
			    {"EOPNOTSUPP", -1},
#endif
#ifdef EOVERFLOW
			    {"EOVERFLOW", static_cast<long>(EOVERFLOW)},
#else
			    {"EOVERFLOW", -1},
#endif
#ifdef EPERM
			    {"EPERM", static_cast<long>(EPERM)},
#else
			    {"EPERM", -1},
#endif
#ifdef EPIPE
			    {"EPIPE", static_cast<long>(EPIPE)},
#else
			    {"EPIPE", -1},
#endif
#ifdef EPROTO
			    {"EPROTO", static_cast<long>(EPROTO)},
#else
			    {"EPROTO", -1},
#endif
#ifdef EPROTONOSUPPORT
			    {"EPROTONOSUPPORT", static_cast<long>(EPROTONOSUPPORT)},
#else
			    {"EPROTONOSUPPORT", -1},
#endif
#ifdef EPROTOTYPE
			    {"EPROTOTYPE", static_cast<long>(EPROTOTYPE)},
#else
			    {"EPROTOTYPE", -1},
#endif
#ifdef ERANGE
			    {"ERANGE", static_cast<long>(ERANGE)},
#else
			    {"ERANGE", -1},
#endif
#ifdef EROFS
			    {"EROFS", static_cast<long>(EROFS)},
#else
			    {"EROFS", -1},
#endif
#ifdef ESPIPE
			    {"ESPIPE", static_cast<long>(ESPIPE)},
#else
			    {"ESPIPE", -1},
#endif
#ifdef ESRCH
			    {"ESRCH", static_cast<long>(ESRCH)},
#else
			    {"ESRCH", -1},
#endif
#ifdef ESTALE
			    {"ESTALE", static_cast<long>(ESTALE)},
#else
			    {"ESTALE", -1},
#endif
#ifdef ETH_P_ALL
			    {"ETH_P_ALL", static_cast<long>(ETH_P_ALL)},
#else
			    {"ETH_P_ALL", -1},
#endif
#ifdef ETH_P_ARP
			    {"ETH_P_ARP", static_cast<long>(ETH_P_ARP)},
#else
			    {"ETH_P_ARP", -1},
#endif
#ifdef ETH_P_IP
			    {"ETH_P_IP", static_cast<long>(ETH_P_IP)},
#else
			    {"ETH_P_IP", -1},
#endif
#ifdef ETH_P_IPV6
			    {"ETH_P_IPV6", static_cast<long>(ETH_P_IPV6)},
#else
			    {"ETH_P_IPV6", -1},
#endif
#ifdef ETIME
			    {"ETIME", static_cast<long>(ETIME)},
#else
			    {"ETIME", -1},
#endif
#ifdef ETIMEDOUT
			    {"ETIMEDOUT", static_cast<long>(ETIMEDOUT)},
#else
			    {"ETIMEDOUT", -1},
#endif
#ifdef ETXTBSY
			    {"ETXTBSY", static_cast<long>(ETXTBSY)},
#else
			    {"ETXTBSY", -1},
#endif
#ifdef EUSERS
			    {"EUSERS", static_cast<long>(EUSERS)},
#else
			    {"EUSERS", -1},
#endif
#ifdef EXDEV
			    {"EXDEV", static_cast<long>(EXDEV)},
#else
			    {"EXDEV", -1},
#endif
#ifdef EXIT_FAILURE
			    {"EXIT_FAILURE", static_cast<long>(EXIT_FAILURE)},
#else
			    {"EXIT_FAILURE", -1},
#endif
#ifdef EXIT_SUCCESS
			    {"EXIT_SUCCESS", static_cast<long>(EXIT_SUCCESS)},
#else
			    {"EXIT_SUCCESS", -1},
#endif
#ifdef FD_CLOEXEC
			    {"FD_CLOEXEC", static_cast<long>(FD_CLOEXEC)},
#else
			    {"FD_CLOEXEC", -1},
#endif
#ifdef FIONREAD
			    {"FIONREAD", static_cast<long>(FIONREAD)},
#else
			    {"FIONREAD", -1},
#endif
#ifdef F_DUPFD
			    {"F_DUPFD", static_cast<long>(F_DUPFD)},
#else
			    {"F_DUPFD", -1},
#endif
#ifdef F_DUPFD_CLOEXEC
			    {"F_DUPFD_CLOEXEC", static_cast<long>(F_DUPFD_CLOEXEC)},
#else
			    {"F_DUPFD_CLOEXEC", -1},
#endif
#ifdef F_GETFD
			    {"F_GETFD", static_cast<long>(F_GETFD)},
#else
			    {"F_GETFD", -1},
#endif
#ifdef F_GETFL
			    {"F_GETFL", static_cast<long>(F_GETFL)},
#else
			    {"F_GETFL", -1},
#endif
#ifdef F_GETLK
			    {"F_GETLK", static_cast<long>(F_GETLK)},
#else
			    {"F_GETLK", -1},
#endif
#ifdef F_GETLK64
			    {"F_GETLK64", static_cast<long>(F_GETLK64)},
#else
			    {"F_GETLK64", -1},
#endif
#ifdef F_GETOWN
			    {"F_GETOWN", static_cast<long>(F_GETOWN)},
#else
			    {"F_GETOWN", -1},
#endif
#ifdef F_OK
			    {"F_OK", static_cast<long>(F_OK)},
#else
			    {"F_OK", -1},
#endif
#ifdef F_RDLCK
			    {"F_RDLCK", static_cast<long>(F_RDLCK)},
#else
			    {"F_RDLCK", -1},
#endif
#ifdef F_SETFD
			    {"F_SETFD", static_cast<long>(F_SETFD)},
#else
			    {"F_SETFD", -1},
#endif
#ifdef F_SETFL
			    {"F_SETFL", static_cast<long>(F_SETFL)},
#else
			    {"F_SETFL", -1},
#endif
#ifdef F_SETLK
			    {"F_SETLK", static_cast<long>(F_SETLK)},
#else
			    {"F_SETLK", -1},
#endif
#ifdef F_SETLK64
			    {"F_SETLK64", static_cast<long>(F_SETLK64)},
#else
			    {"F_SETLK64", -1},
#endif
#ifdef F_SETLKW
			    {"F_SETLKW", static_cast<long>(F_SETLKW)},
#else
			    {"F_SETLKW", -1},
#endif
#ifdef F_SETLKW64
			    {"F_SETLKW64", static_cast<long>(F_SETLKW64)},
#else
			    {"F_SETLKW64", -1},
#endif
#ifdef F_SETOWN
			    {"F_SETOWN", static_cast<long>(F_SETOWN)},
#else
			    {"F_SETOWN", -1},
#endif
#ifdef F_UNLCK
			    {"F_UNLCK", static_cast<long>(F_UNLCK)},
#else
			    {"F_UNLCK", -1},
#endif
#ifdef F_WRLCK
			    {"F_WRLCK", static_cast<long>(F_WRLCK)},
#else
			    {"F_WRLCK", -1},
#endif
#ifdef ICMP_ECHO
			    {"ICMP_ECHO", static_cast<long>(ICMP_ECHO)},
#else
			    {"ICMP_ECHO", -1},
#endif
#ifdef ICMP_ECHOREPLY
			    {"ICMP_ECHOREPLY", static_cast<long>(ICMP_ECHOREPLY)},
#else
			    {"ICMP_ECHOREPLY", -1},
#endif
#ifdef ICMP6_ECHO_REQUEST
			    {"ICMP6_ECHO_REQUEST", static_cast<long>(ICMP6_ECHO_REQUEST)},
#else
			    {"ICMP6_ECHO_REQUEST", -1},
#endif
#ifdef ICMP6_ECHO_REPLY
			    {"ICMP6_ECHO_REPLY", static_cast<long>(ICMP6_ECHO_REPLY)},
#else
			    {"ICMP6_ECHO_REPLY", -1},
#endif
#ifdef IFA_F_DADFAILED
			    {"IFA_F_DADFAILED", static_cast<long>(IFA_F_DADFAILED)},
#else
			    {"IFA_F_DADFAILED", -1},
#endif
#ifdef IFA_F_DEPRECATED
			    {"IFA_F_DEPRECATED", static_cast<long>(IFA_F_DEPRECATED)},
#else
			    {"IFA_F_DEPRECATED", -1},
#endif
#ifdef IFA_F_HOMEADDRESS
			    {"IFA_F_HOMEADDRESS", static_cast<long>(IFA_F_HOMEADDRESS)},
#else
			    {"IFA_F_HOMEADDRESS", -1},
#endif
#ifdef IFA_F_NODAD
			    {"IFA_F_NODAD", static_cast<long>(IFA_F_NODAD)},
#else
			    {"IFA_F_NODAD", -1},
#endif
#ifdef IFA_F_OPTIMISTIC
			    {"IFA_F_OPTIMISTIC", static_cast<long>(IFA_F_OPTIMISTIC)},
#else
			    {"IFA_F_OPTIMISTIC", -1},
#endif
#ifdef IFA_F_PERMANENT
			    {"IFA_F_PERMANENT", static_cast<long>(IFA_F_PERMANENT)},
#else
			    {"IFA_F_PERMANENT", -1},
#endif
#ifdef IFA_F_SECONDARY
			    {"IFA_F_SECONDARY", static_cast<long>(IFA_F_SECONDARY)},
#else
			    {"IFA_F_SECONDARY", -1},
#endif
#ifdef IFA_F_TEMPORARY
			    {"IFA_F_TEMPORARY", static_cast<long>(IFA_F_TEMPORARY)},
#else
			    {"IFA_F_TEMPORARY", -1},
#endif
#ifdef IFA_F_TENTATIVE
			    {"IFA_F_TENTATIVE", static_cast<long>(IFA_F_TENTATIVE)},
#else
			    {"IFA_F_TENTATIVE", -1},
#endif
#ifdef IFF_ALLMULTI
			    {"IFF_ALLMULTI", static_cast<long>(IFF_ALLMULTI)},
#else
			    {"IFF_ALLMULTI", -1},
#endif
#ifdef IFF_AUTOMEDIA
			    {"IFF_AUTOMEDIA", static_cast<long>(IFF_AUTOMEDIA)},
#else
			    {"IFF_AUTOMEDIA", -1},
#endif
#ifdef IFF_BROADCAST
			    {"IFF_BROADCAST", static_cast<long>(IFF_BROADCAST)},
#else
			    {"IFF_BROADCAST", -1},
#endif
#ifdef IFF_DEBUG
			    {"IFF_DEBUG", static_cast<long>(IFF_DEBUG)},
#else
			    {"IFF_DEBUG", -1},
#endif
#ifdef IFF_DYNAMIC
			    {"IFF_DYNAMIC", static_cast<long>(IFF_DYNAMIC)},
#else
			    {"IFF_DYNAMIC", -1},
#endif
#ifdef IFF_LOOPBACK
			    {"IFF_LOOPBACK", static_cast<long>(IFF_LOOPBACK)},
#else
			    {"IFF_LOOPBACK", -1},
#endif
#ifdef IFF_MASTER
			    {"IFF_MASTER", static_cast<long>(IFF_MASTER)},
#else
			    {"IFF_MASTER", -1},
#endif
#ifdef IFF_MULTICAST
			    {"IFF_MULTICAST", static_cast<long>(IFF_MULTICAST)},
#else
			    {"IFF_MULTICAST", -1},
#endif
#ifdef IFF_NOARP
			    {"IFF_NOARP", static_cast<long>(IFF_NOARP)},
#else
			    {"IFF_NOARP", -1},
#endif
#ifdef IFF_NOTRAILERS
			    {"IFF_NOTRAILERS", static_cast<long>(IFF_NOTRAILERS)},
#else
			    {"IFF_NOTRAILERS", -1},
#endif
#ifdef IFF_POINTOPOINT
			    {"IFF_POINTOPOINT", static_cast<long>(IFF_POINTOPOINT)},
#else
			    {"IFF_POINTOPOINT", -1},
#endif
#ifdef IFF_PORTSEL
			    {"IFF_PORTSEL", static_cast<long>(IFF_PORTSEL)},
#else
			    {"IFF_PORTSEL", -1},
#endif
#ifdef IFF_PROMISC
			    {"IFF_PROMISC", static_cast<long>(IFF_PROMISC)},
#else
			    {"IFF_PROMISC", -1},
#endif
#ifdef IFF_RUNNING
			    {"IFF_RUNNING", static_cast<long>(IFF_RUNNING)},
#else
			    {"IFF_RUNNING", -1},
#endif
#ifdef IFF_SLAVE
			    {"IFF_SLAVE", static_cast<long>(IFF_SLAVE)},
#else
			    {"IFF_SLAVE", -1},
#endif
#ifdef IFF_UP
			    {"IFF_UP", static_cast<long>(IFF_UP)},
#else
			    {"IFF_UP", -1},
#endif
#ifdef IPPROTO_ICMP
			    {"IPPROTO_ICMP", static_cast<long>(IPPROTO_ICMP)},
#else
			    {"IPPROTO_ICMP", -1},
#endif
#ifdef IPPROTO_ICMPV6
			    {"IPPROTO_ICMPV6", static_cast<long>(IPPROTO_ICMPV6)},
#else
			    {"IPPROTO_ICMPV6", -1},
#endif
#ifdef IPPROTO_IP
			    {"IPPROTO_IP", static_cast<long>(IPPROTO_IP)},
#else
			    {"IPPROTO_IP", -1},
#endif
#ifdef IPPROTO_IPV6
			    {"IPPROTO_IPV6", static_cast<long>(IPPROTO_IPV6)},
#else
			    {"IPPROTO_IPV6", -1},
#endif
#ifdef IPPROTO_RAW
			    {"IPPROTO_RAW", static_cast<long>(IPPROTO_RAW)},
#else
			    {"IPPROTO_RAW", -1},
#endif
#ifdef IPPROTO_TCP
			    {"IPPROTO_TCP", static_cast<long>(IPPROTO_TCP)},
#else
			    {"IPPROTO_TCP", -1},
#endif
#ifdef IPPROTO_UDP
			    {"IPPROTO_UDP", static_cast<long>(IPPROTO_UDP)},
#else
			    {"IPPROTO_UDP", -1},
#endif
#ifdef IPV6_CHECKSUM
			    {"IPV6_CHECKSUM", static_cast<long>(IPV6_CHECKSUM)},
#else
			    {"IPV6_CHECKSUM", -1},
#endif
#ifdef IPV6_MULTICAST_HOPS
			    {"IPV6_MULTICAST_HOPS", static_cast<long>(IPV6_MULTICAST_HOPS)},
#else
			    {"IPV6_MULTICAST_HOPS", -1},
#endif
#ifdef IPV6_MULTICAST_IF
			    {"IPV6_MULTICAST_IF", static_cast<long>(IPV6_MULTICAST_IF)},
#else
			    {"IPV6_MULTICAST_IF", -1},
#endif
#ifdef IPV6_MULTICAST_LOOP
			    {"IPV6_MULTICAST_LOOP", static_cast<long>(IPV6_MULTICAST_LOOP)},
#else
			    {"IPV6_MULTICAST_LOOP", -1},
#endif
#ifdef IPV6_RECVDSTOPTS
			    {"IPV6_RECVDSTOPTS", static_cast<long>(IPV6_RECVDSTOPTS)},
#else
			    {"IPV6_RECVDSTOPTS", -1},
#endif
#ifdef IPV6_RECVHOPLIMIT
			    {"IPV6_RECVHOPLIMIT", static_cast<long>(IPV6_RECVHOPLIMIT)},
#else
			    {"IPV6_RECVHOPLIMIT", -1},
#endif
#ifdef IPV6_RECVHOPOPTS
			    {"IPV6_RECVHOPOPTS", static_cast<long>(IPV6_RECVHOPOPTS)},
#else
			    {"IPV6_RECVHOPOPTS", -1},
#endif
#ifdef IPV6_RECVPKTINFO
			    {"IPV6_RECVPKTINFO", static_cast<long>(IPV6_RECVPKTINFO)},
#else
			    {"IPV6_RECVPKTINFO", -1},
#endif
#ifdef IPV6_RECVRTHDR
			    {"IPV6_RECVRTHDR", static_cast<long>(IPV6_RECVRTHDR)},
#else
			    {"IPV6_RECVRTHDR", -1},
#endif
#ifdef IPV6_RECVTCLASS
			    {"IPV6_RECVTCLASS", static_cast<long>(IPV6_RECVTCLASS)},
#else
			    {"IPV6_RECVTCLASS", -1},
#endif
#ifdef IPV6_TCLASS
			    {"IPV6_TCLASS", static_cast<long>(IPV6_TCLASS)},
#else
			    {"IPV6_TCLASS", -1},
#endif
#ifdef IPV6_UNICAST_HOPS
			    {"IPV6_UNICAST_HOPS", static_cast<long>(IPV6_UNICAST_HOPS)},
#else
			    {"IPV6_UNICAST_HOPS", -1},
#endif
#ifdef IPV6_V6ONLY
			    {"IPV6_V6ONLY", static_cast<long>(IPV6_V6ONLY)},
#else
			    {"IPV6_V6ONLY", -1},
#endif
#ifdef IP_MULTICAST_ALL
			    {"IP_MULTICAST_ALL", static_cast<long>(IP_MULTICAST_ALL)},
#else
			    {"IP_MULTICAST_ALL", -1},
#endif
#ifdef IP_MULTICAST_IF
			    {"IP_MULTICAST_IF", static_cast<long>(IP_MULTICAST_IF)},
#else
			    {"IP_MULTICAST_IF", -1},
#endif
#ifdef IP_MULTICAST_LOOP
			    {"IP_MULTICAST_LOOP", static_cast<long>(IP_MULTICAST_LOOP)},
#else
			    {"IP_MULTICAST_LOOP", -1},
#endif
#ifdef IP_MULTICAST_TTL
			    {"IP_MULTICAST_TTL", static_cast<long>(IP_MULTICAST_TTL)},
#else
			    {"IP_MULTICAST_TTL", -1},
#endif
#ifdef IP_RECVTOS
			    {"IP_RECVTOS", static_cast<long>(IP_RECVTOS)},
#else
			    {"IP_RECVTOS", -1},
#endif
#ifdef IP_TOS
			    {"IP_TOS", static_cast<long>(IP_TOS)},
#else
			    {"IP_TOS", -1},
#endif
#ifdef IP_TTL
			    {"IP_TTL", static_cast<long>(IP_TTL)},
#else
			    {"IP_TTL", -1},
#endif
#ifdef _LINUX_CAPABILITY_VERSION_3
			    {"_LINUX_CAPABILITY_VERSION_3", static_cast<long>(_LINUX_CAPABILITY_VERSION_3)},
#else
			    {"_LINUX_CAPABILITY_VERSION_3", -1},
#endif
#ifdef MAP_FIXED
			    {"MAP_FIXED", static_cast<long>(MAP_FIXED)},
#else
			    {"MAP_FIXED", -1},
#endif
#ifdef MAP_ANONYMOUS
			    {"MAP_ANONYMOUS", static_cast<long>(MAP_ANONYMOUS)},
#else
			    {"MAP_ANONYMOUS", -1},
#endif
#ifdef MAP_POPULATE
			    {"MAP_POPULATE", static_cast<long>(MAP_POPULATE)},
#else
			    {"MAP_POPULATE", -1},
#endif
#ifdef MAP_PRIVATE
			    {"MAP_PRIVATE", static_cast<long>(MAP_PRIVATE)},
#else
			    {"MAP_PRIVATE", -1},
#endif
#ifdef MAP_SHARED
			    {"MAP_SHARED", static_cast<long>(MAP_SHARED)},
#else
			    {"MAP_SHARED", -1},
#endif
#ifdef MCAST_JOIN_GROUP
			    {"MCAST_JOIN_GROUP", static_cast<long>(MCAST_JOIN_GROUP)},
#else
			    {"MCAST_JOIN_GROUP", -1},
#endif
#ifdef MCAST_LEAVE_GROUP
			    {"MCAST_LEAVE_GROUP", static_cast<long>(MCAST_LEAVE_GROUP)},
#else
			    {"MCAST_LEAVE_GROUP", -1},
#endif
#ifdef MCAST_JOIN_SOURCE_GROUP
			    {"MCAST_JOIN_SOURCE_GROUP", static_cast<long>(MCAST_JOIN_SOURCE_GROUP)},
#else
			    {"MCAST_JOIN_SOURCE_GROUP", -1},
#endif
#ifdef MCAST_LEAVE_SOURCE_GROUP
			    {"MCAST_LEAVE_SOURCE_GROUP", static_cast<long>(MCAST_LEAVE_SOURCE_GROUP)},
#else
			    {"MCAST_LEAVE_SOURCE_GROUP", -1},
#endif
#ifdef MCAST_BLOCK_SOURCE
			    {"MCAST_BLOCK_SOURCE", static_cast<long>(MCAST_BLOCK_SOURCE)},
#else
			    {"MCAST_BLOCK_SOURCE", -1},
#endif
#ifdef MCAST_UNBLOCK_SOURCE
			    {"MCAST_UNBLOCK_SOURCE", static_cast<long>(MCAST_UNBLOCK_SOURCE)},
#else
			    {"MCAST_UNBLOCK_SOURCE", -1},
#endif
#ifdef MCL_CURRENT
			    {"MCL_CURRENT", static_cast<long>(MCL_CURRENT)},
#else
			    {"MCL_CURRENT", -1},
#endif
#ifdef MCL_FUTURE
			    {"MCL_FUTURE", static_cast<long>(MCL_FUTURE)},
#else
			    {"MCL_FUTURE", -1},
#endif
#ifdef MFD_CLOEXEC
			    {"MFD_CLOEXEC", static_cast<long>(MFD_CLOEXEC)},
#else
			    {"MFD_CLOEXEC", -1},
#endif
#ifdef MSG_CTRUNC
			    {"MSG_CTRUNC", static_cast<long>(MSG_CTRUNC)},
#else
			    {"MSG_CTRUNC", -1},
#endif
#ifdef MSG_DONTROUTE
			    {"MSG_DONTROUTE", static_cast<long>(MSG_DONTROUTE)},
#else
			    {"MSG_DONTROUTE", -1},
#endif
#ifdef MSG_EOR
			    {"MSG_EOR", static_cast<long>(MSG_EOR)},
#else
			    {"MSG_EOR", -1},
#endif
#ifdef MSG_OOB
			    {"MSG_OOB", static_cast<long>(MSG_OOB)},
#else
			    {"MSG_OOB", -1},
#endif
#ifdef MSG_PEEK
			    {"MSG_PEEK", static_cast<long>(MSG_PEEK)},
#else
			    {"MSG_PEEK", -1},
#endif
#ifdef MSG_TRUNC
			    {"MSG_TRUNC", static_cast<long>(MSG_TRUNC)},
#else
			    {"MSG_TRUNC", -1},
#endif
#ifdef MSG_WAITALL
			    {"MSG_WAITALL", static_cast<long>(MSG_WAITALL)},
#else
			    {"MSG_WAITALL", -1},
#endif
#ifdef MS_ASYNC
			    {"MS_ASYNC", static_cast<long>(MS_ASYNC)},
#else
			    {"MS_ASYNC", -1},
#endif
#ifdef MS_INVALIDATE
			    {"MS_INVALIDATE", static_cast<long>(MS_INVALIDATE)},
#else
			    {"MS_INVALIDATE", -1},
#endif
#ifdef MS_SYNC
			    {"MS_SYNC", static_cast<long>(MS_SYNC)},
#else
			    {"MS_SYNC", -1},
#endif
#ifdef NETLINK_NETFILTER
			    {"NETLINK_NETFILTER", static_cast<long>(NETLINK_NETFILTER)},
#else
			    {"NETLINK_NETFILTER", -1},
#endif
#ifdef NETLINK_ROUTE
			    {"NETLINK_ROUTE", static_cast<long>(NETLINK_ROUTE)},
#else
			    {"NETLINK_ROUTE", -1},
#endif
#ifdef NETLINK_INET_DIAG
			    {"NETLINK_INET_DIAG", static_cast<long>(NETLINK_INET_DIAG)},
#else
			    {"NETLINK_INET_DIAG", -1},
#endif
#ifdef NI_DGRAM
			    {"NI_DGRAM", static_cast<long>(NI_DGRAM)},
#else
			    {"NI_DGRAM", -1},
#endif
#ifdef NI_NAMEREQD
			    {"NI_NAMEREQD", static_cast<long>(NI_NAMEREQD)},
#else
			    {"NI_NAMEREQD", -1},
#endif
#ifdef NI_NOFQDN
			    {"NI_NOFQDN", static_cast<long>(NI_NOFQDN)},
#else
			    {"NI_NOFQDN", -1},
#endif
#ifdef NI_NUMERICHOST
			    {"NI_NUMERICHOST", static_cast<long>(NI_NUMERICHOST)},
#else
			    {"NI_NUMERICHOST", -1},
#endif
#ifdef NI_NUMERICSERV
			    {"NI_NUMERICSERV", static_cast<long>(NI_NUMERICSERV)},
#else
			    {"NI_NUMERICSERV", -1},
#endif
#ifdef O_ACCMODE
			    {"O_ACCMODE", static_cast<long>(O_ACCMODE)},
#else
			    {"O_ACCMODE", -1},
#endif
#ifdef O_APPEND
			    {"O_APPEND", static_cast<long>(O_APPEND)},
#else
			    {"O_APPEND", -1},
#endif
#ifdef O_CLOEXEC
			    {"O_CLOEXEC", static_cast<long>(O_CLOEXEC)},
#else
			    {"O_CLOEXEC", -1},
#endif
#ifdef O_CREAT
			    {"O_CREAT", static_cast<long>(O_CREAT)},
#else
			    {"O_CREAT", -1},
#endif
#ifdef O_DIRECT
			    {"O_DIRECT", static_cast<long>(O_DIRECT)},
#else
			    {"O_DIRECT", -1},
#endif
#ifdef O_EXCL
			    {"O_EXCL", static_cast<long>(O_EXCL)},
#else
			    {"O_EXCL", -1},
#endif
#ifdef O_NOCTTY
			    {"O_NOCTTY", static_cast<long>(O_NOCTTY)},
#else
			    {"O_NOCTTY", -1},
#endif
#ifdef O_NOFOLLOW
			    {"O_NOFOLLOW", static_cast<long>(O_NOFOLLOW)},
#else
			    {"O_NOFOLLOW", -1},
#endif
#ifdef O_NONBLOCK
			    {"O_NONBLOCK", static_cast<long>(O_NONBLOCK)},
#else
			    {"O_NONBLOCK", -1},
#endif
#ifdef O_RDONLY
			    {"O_RDONLY", static_cast<long>(O_RDONLY)},
#else
			    {"O_RDONLY", -1},
#endif
#ifdef O_RDWR
			    {"O_RDWR", static_cast<long>(O_RDWR)},
#else
			    {"O_RDWR", -1},
#endif
#ifdef O_SYNC
			    {"O_SYNC", static_cast<long>(O_SYNC)},
#else
			    {"O_SYNC", -1},
#endif
#ifdef O_DSYNC
			    {"O_DSYNC", static_cast<long>(O_DSYNC)},
#else
			    {"O_DSYNC", -1},
#endif
#ifdef O_TRUNC
			    {"O_TRUNC", static_cast<long>(O_TRUNC)},
#else
			    {"O_TRUNC", -1},
#endif
#ifdef O_WRONLY
			    {"O_WRONLY", static_cast<long>(O_WRONLY)},
#else
			    {"O_WRONLY", -1},
#endif
#ifdef POLLERR
			    {"POLLERR", static_cast<long>(POLLERR)},
#else
			    {"POLLERR", -1},
#endif
#ifdef POLLHUP
			    {"POLLHUP", static_cast<long>(POLLHUP)},
#else
			    {"POLLHUP", -1},
#endif
#ifdef POLLIN
			    {"POLLIN", static_cast<long>(POLLIN)},
#else
			    {"POLLIN", -1},
#endif
#ifdef POLLNVAL
			    {"POLLNVAL", static_cast<long>(POLLNVAL)},
#else
			    {"POLLNVAL", -1},
#endif
#ifdef POLLOUT
			    {"POLLOUT", static_cast<long>(POLLOUT)},
#else
			    {"POLLOUT", -1},
#endif
#ifdef POLLPRI
			    {"POLLPRI", static_cast<long>(POLLPRI)},
#else
			    {"POLLPRI", -1},
#endif
#ifdef POLLRDBAND
			    {"POLLRDBAND", static_cast<long>(POLLRDBAND)},
#else
			    {"POLLRDBAND", -1},
#endif
#ifdef POLLRDNORM
			    {"POLLRDNORM", static_cast<long>(POLLRDNORM)},
#else
			    {"POLLRDNORM", -1},
#endif
#ifdef POLLWRBAND
			    {"POLLWRBAND", static_cast<long>(POLLWRBAND)},
#else
			    {"POLLWRBAND", -1},
#endif
#ifdef POLLWRNORM
			    {"POLLWRNORM", static_cast<long>(POLLWRNORM)},
#else
			    {"POLLWRNORM", -1},
#endif
#ifdef PR_CAP_AMBIENT
			    {"PR_CAP_AMBIENT", static_cast<long>(PR_CAP_AMBIENT)},
#else
			    {"PR_CAP_AMBIENT", -1},
#endif
#ifdef PR_CAP_AMBIENT_RAISE
			    {"PR_CAP_AMBIENT_RAISE", static_cast<long>(PR_CAP_AMBIENT_RAISE)},
#else
			    {"PR_CAP_AMBIENT_RAISE", -1},
#endif
#ifdef PR_GET_DUMPABLE
			    {"PR_GET_DUMPABLE", static_cast<long>(PR_GET_DUMPABLE)},
#else
			    {"PR_GET_DUMPABLE", -1},
#endif
#ifdef PR_SET_DUMPABLE
			    {"PR_SET_DUMPABLE", static_cast<long>(PR_SET_DUMPABLE)},
#else
			    {"PR_SET_DUMPABLE", -1},
#endif
#ifdef PR_SET_NO_NEW_PRIVS
			    {"PR_SET_NO_NEW_PRIVS", static_cast<long>(PR_SET_NO_NEW_PRIVS)},
#else
			    {"PR_SET_NO_NEW_PRIVS", -1},
#endif
#ifdef PROT_EXEC
			    {"PROT_EXEC", static_cast<long>(PROT_EXEC)},
#else
			    {"PROT_EXEC", -1},
#endif
#ifdef PROT_NONE
			    {"PROT_NONE", static_cast<long>(PROT_NONE)},
#else
			    {"PROT_NONE", -1},
#endif
#ifdef PROT_READ
			    {"PROT_READ", static_cast<long>(PROT_READ)},
#else
			    {"PROT_READ", -1},
#endif
#ifdef PROT_WRITE
			    {"PROT_WRITE", static_cast<long>(PROT_WRITE)},
#else
			    {"PROT_WRITE", -1},
#endif
#ifdef R_OK
			    {"R_OK", static_cast<long>(R_OK)},
#else
			    {"R_OK", -1},
#endif
#ifdef RLIMIT_NOFILE
			    {"RLIMIT_NOFILE", static_cast<long>(RLIMIT_NOFILE)},
#else
			    {"RLIMIT_NOFILE", -1},
#endif
#ifdef RT_SCOPE_HOST
			    {"RT_SCOPE_HOST", static_cast<long>(RT_SCOPE_HOST)},
#else
			    {"RT_SCOPE_HOST", -1},
#endif
#ifdef RT_SCOPE_LINK
			    {"RT_SCOPE_LINK", static_cast<long>(RT_SCOPE_LINK)},
#else
			    {"RT_SCOPE_LINK", -1},
#endif
#ifdef RT_SCOPE_NOWHERE
			    {"RT_SCOPE_NOWHERE", static_cast<long>(RT_SCOPE_NOWHERE)},
#else
			    {"RT_SCOPE_NOWHERE", -1},
#endif
#ifdef RT_SCOPE_SITE
			    {"RT_SCOPE_SITE", static_cast<long>(RT_SCOPE_SITE)},
#else
			    {"RT_SCOPE_SITE", -1},
#endif
#ifdef RT_SCOPE_UNIVERSE
			    {"RT_SCOPE_UNIVERSE", static_cast<long>(RT_SCOPE_UNIVERSE)},
#else
			    {"RT_SCOPE_UNIVERSE", -1},
#endif
#ifdef RTMGRP_IPV4_IFADDR
			    {"RTMGRP_IPV4_IFADDR", static_cast<long>(RTMGRP_IPV4_IFADDR)},
#else
			    {"RTMGRP_IPV4_IFADDR", -1},
#endif
#ifdef RTMGRP_IPV4_MROUTE
			    {"RTMGRP_IPV4_MROUTE", static_cast<long>(RTMGRP_IPV4_MROUTE)},
#else
			    {"RTMGRP_IPV4_MROUTE", -1},
#endif
#ifdef RTMGRP_IPV4_ROUTE
			    {"RTMGRP_IPV4_ROUTE", static_cast<long>(RTMGRP_IPV4_ROUTE)},
#else
			    {"RTMGRP_IPV4_ROUTE", -1},
#endif
#ifdef RTMGRP_IPV4_RULE
			    {"RTMGRP_IPV4_RULE", static_cast<long>(RTMGRP_IPV4_RULE)},
#else
			    {"RTMGRP_IPV4_RULE", -1},
#endif
#ifdef RTMGRP_IPV6_IFADDR
			    {"RTMGRP_IPV6_IFADDR", static_cast<long>(RTMGRP_IPV6_IFADDR)},
#else
			    {"RTMGRP_IPV6_IFADDR", -1},
#endif
#ifdef RTMGRP_IPV6_IFINFO
			    {"RTMGRP_IPV6_IFINFO", static_cast<long>(RTMGRP_IPV6_IFINFO)},
#else
			    {"RTMGRP_IPV6_IFINFO", -1},
#endif
#ifdef RTMGRP_IPV6_MROUTE
			    {"RTMGRP_IPV6_MROUTE", static_cast<long>(RTMGRP_IPV6_MROUTE)},
#else
			    {"RTMGRP_IPV6_MROUTE", -1},
#endif
#ifdef RTMGRP_IPV6_PREFIX
			    {"RTMGRP_IPV6_PREFIX", static_cast<long>(RTMGRP_IPV6_PREFIX)},
#else
			    {"RTMGRP_IPV6_PREFIX", -1},
#endif
#ifdef RTMGRP_IPV6_ROUTE
			    {"RTMGRP_IPV6_ROUTE", static_cast<long>(RTMGRP_IPV6_ROUTE)},
#else
			    {"RTMGRP_IPV6_ROUTE", -1},
#endif
#ifdef RTMGRP_LINK
			    {"RTMGRP_LINK", static_cast<long>(RTMGRP_LINK)},
#else
			    {"RTMGRP_LINK", -1},
#endif
#ifdef RTMGRP_NEIGH
			    {"RTMGRP_NEIGH", static_cast<long>(RTMGRP_NEIGH)},
#else
			    {"RTMGRP_NEIGH", -1},
#endif
#ifdef RTMGRP_NOTIFY
			    {"RTMGRP_NOTIFY", static_cast<long>(RTMGRP_NOTIFY)},
#else
			    {"RTMGRP_NOTIFY", -1},
#endif
#ifdef RTMGRP_TC
			    {"RTMGRP_TC", static_cast<long>(RTMGRP_TC)},
#else
			    {"RTMGRP_TC", -1},
#endif
#ifdef SEEK_CUR
			    {"SEEK_CUR", static_cast<long>(SEEK_CUR)},
#else
			    {"SEEK_CUR", -1},
#endif
#ifdef SEEK_END
			    {"SEEK_END", static_cast<long>(SEEK_END)},
#else
			    {"SEEK_END", -1},
#endif
#ifdef SEEK_SET
			    {"SEEK_SET", static_cast<long>(SEEK_SET)},
#else
			    {"SEEK_SET", -1},
#endif
#ifdef SHUT_RD
			    {"SHUT_RD", static_cast<long>(SHUT_RD)},
#else
			    {"SHUT_RD", -1},
#endif
#ifdef SHUT_RDWR
			    {"SHUT_RDWR", static_cast<long>(SHUT_RDWR)},
#else
			    {"SHUT_RDWR", -1},
#endif
#ifdef SHUT_WR
			    {"SHUT_WR", static_cast<long>(SHUT_WR)},
#else
			    {"SHUT_WR", -1},
#endif
#ifdef SIGABRT
			    {"SIGABRT", static_cast<long>(SIGABRT)},
#else
			    {"SIGABRT", -1},
#endif
#ifdef SIGALRM
			    {"SIGALRM", static_cast<long>(SIGALRM)},
#else
			    {"SIGALRM", -1},
#endif
#ifdef SIGBUS
			    {"SIGBUS", static_cast<long>(SIGBUS)},
#else
			    {"SIGBUS", -1},
#endif
#ifdef SIGCHLD
			    {"SIGCHLD", static_cast<long>(SIGCHLD)},
#else
			    {"SIGCHLD", -1},
#endif
#ifdef SIGCONT
			    {"SIGCONT", static_cast<long>(SIGCONT)},
#else
			    {"SIGCONT", -1},
#endif
#ifdef SIGFPE
			    {"SIGFPE", static_cast<long>(SIGFPE)},
#else
			    {"SIGFPE", -1},
#endif
#ifdef SIGHUP
			    {"SIGHUP", static_cast<long>(SIGHUP)},
#else
			    {"SIGHUP", -1},
#endif
#ifdef SIGILL
			    {"SIGILL", static_cast<long>(SIGILL)},
#else
			    {"SIGILL", -1},
#endif
#ifdef SIGINT
			    {"SIGINT", static_cast<long>(SIGINT)},
#else
			    {"SIGINT", -1},
#endif
#ifdef SIGIO
			    {"SIGIO", static_cast<long>(SIGIO)},
#else
			    {"SIGIO", -1},
#endif
#ifdef SIGKILL
			    {"SIGKILL", static_cast<long>(SIGKILL)},
#else
			    {"SIGKILL", -1},
#endif
#ifdef SIGPIPE
			    {"SIGPIPE", static_cast<long>(SIGPIPE)},
#else
			    {"SIGPIPE", -1},
#endif
#ifdef SIGPROF
			    {"SIGPROF", static_cast<long>(SIGPROF)},
#else
			    {"SIGPROF", -1},
#endif
#ifdef SIGPWR
			    {"SIGPWR", static_cast<long>(SIGPWR)},
#else
			    {"SIGPWR", -1},
#endif
#ifdef SIGQUIT
			    {"SIGQUIT", static_cast<long>(SIGQUIT)},
#else
			    {"SIGQUIT", -1},
#endif
#ifdef SIGRTMAX
			    {"SIGRTMAX", static_cast<long>(SIGRTMAX)},
#else
			    {"SIGRTMAX", -1},
#endif
#ifdef SIGRTMIN
			    {"SIGRTMIN", static_cast<long>(SIGRTMIN)},
#else
			    {"SIGRTMIN", -1},
#endif
#ifdef SIGSEGV
			    {"SIGSEGV", static_cast<long>(SIGSEGV)},
#else
			    {"SIGSEGV", -1},
#endif
#ifdef SIGSTKFLT
			    {"SIGSTKFLT", static_cast<long>(SIGSTKFLT)},
#else
			    {"SIGSTKFLT", -1},
#endif
#ifdef SIGSTOP
			    {"SIGSTOP", static_cast<long>(SIGSTOP)},
#else
			    {"SIGSTOP", -1},
#endif
#ifdef SIGSYS
			    {"SIGSYS", static_cast<long>(SIGSYS)},
#else
			    {"SIGSYS", -1},
#endif
#ifdef SIGTERM
			    {"SIGTERM", static_cast<long>(SIGTERM)},
#else
			    {"SIGTERM", -1},
#endif
#ifdef SIGTRAP
			    {"SIGTRAP", static_cast<long>(SIGTRAP)},
#else
			    {"SIGTRAP", -1},
#endif
#ifdef SIGTSTP
			    {"SIGTSTP", static_cast<long>(SIGTSTP)},
#else
			    {"SIGTSTP", -1},
#endif
#ifdef SIGTTIN
			    {"SIGTTIN", static_cast<long>(SIGTTIN)},
#else
			    {"SIGTTIN", -1},
#endif
#ifdef SIGTTOU
			    {"SIGTTOU", static_cast<long>(SIGTTOU)},
#else
			    {"SIGTTOU", -1},
#endif
#ifdef SIGURG
			    {"SIGURG", static_cast<long>(SIGURG)},
#else
			    {"SIGURG", -1},
#endif
#ifdef SIGUSR1
			    {"SIGUSR1", static_cast<long>(SIGUSR1)},
#else
			    {"SIGUSR1", -1},
#endif
#ifdef SIGUSR2
			    {"SIGUSR2", static_cast<long>(SIGUSR2)},
#else
			    {"SIGUSR2", -1},
#endif
#ifdef SIGVTALRM
			    {"SIGVTALRM", static_cast<long>(SIGVTALRM)},
#else
			    {"SIGVTALRM", -1},
#endif
#ifdef SIGWINCH
			    {"SIGWINCH", static_cast<long>(SIGWINCH)},
#else
			    {"SIGWINCH", -1},
#endif
#ifdef SIGXCPU
			    {"SIGXCPU", static_cast<long>(SIGXCPU)},
#else
			    {"SIGXCPU", -1},
#endif
#ifdef SIGXFSZ
			    {"SIGXFSZ", static_cast<long>(SIGXFSZ)},
#else
			    {"SIGXFSZ", -1},
#endif
#ifdef SIOCGIFADDR
			    {"SIOCGIFADDR", static_cast<long>(SIOCGIFADDR)},
#else
			    {"SIOCGIFADDR", -1},
#endif
#ifdef SIOCGIFBRDADDR
			    {"SIOCGIFBRDADDR", static_cast<long>(SIOCGIFBRDADDR)},
#else
			    {"SIOCGIFBRDADDR", -1},
#endif
#ifdef SIOCGIFDSTADDR
			    {"SIOCGIFDSTADDR", static_cast<long>(SIOCGIFDSTADDR)},
#else
			    {"SIOCGIFDSTADDR", -1},
#endif
#ifdef SIOCGIFNETMASK
			    {"SIOCGIFNETMASK", static_cast<long>(SIOCGIFNETMASK)},
#else
			    {"SIOCGIFNETMASK", -1},
#endif
#ifdef SOCK_CLOEXEC
			    {"SOCK_CLOEXEC", static_cast<long>(SOCK_CLOEXEC)},
#else
			    {"SOCK_CLOEXEC", -1},
#endif
#ifdef SOCK_DGRAM
			    {"SOCK_DGRAM", static_cast<long>(SOCK_DGRAM)},
#else
			    {"SOCK_DGRAM", -1},
#endif
#ifdef SOCK_NONBLOCK
			    {"SOCK_NONBLOCK", static_cast<long>(SOCK_NONBLOCK)},
#else
			    {"SOCK_NONBLOCK", -1},
#endif
#ifdef SOCK_RAW
			    {"SOCK_RAW", static_cast<long>(SOCK_RAW)},
#else
			    {"SOCK_RAW", -1},
#endif
#ifdef SOCK_SEQPACKET
			    {"SOCK_SEQPACKET", static_cast<long>(SOCK_SEQPACKET)},
#else
			    {"SOCK_SEQPACKET", -1},
#endif
#ifdef SOCK_STREAM
			    {"SOCK_STREAM", static_cast<long>(SOCK_STREAM)},
#else
			    {"SOCK_STREAM", -1},
#endif
#ifdef SOL_SOCKET
			    {"SOL_SOCKET", static_cast<long>(SOL_SOCKET)},
#else
			    {"SOL_SOCKET", -1},
#endif
#ifdef SOL_UDP
			    {"SOL_UDP", static_cast<long>(SOL_UDP)},
#else
			    {"SOL_UDP", -1},
#endif
#ifdef SO_BINDTODEVICE
			    {"SO_BINDTODEVICE", static_cast<long>(SO_BINDTODEVICE)},
#else
			    {"SO_BINDTODEVICE", -1},
#endif
#ifdef SO_BROADCAST
			    {"SO_BROADCAST", static_cast<long>(SO_BROADCAST)},
#else
			    {"SO_BROADCAST", -1},
#endif
#ifdef SO_DEBUG
			    {"SO_DEBUG", static_cast<long>(SO_DEBUG)},
#else
			    {"SO_DEBUG", -1},
#endif
#ifdef SO_DOMAIN
			    {"SO_DOMAIN", static_cast<long>(SO_DOMAIN)},
#else
			    {"SO_DOMAIN", -1},
#endif
#ifdef SO_DONTROUTE
			    {"SO_DONTROUTE", static_cast<long>(SO_DONTROUTE)},
#else
			    {"SO_DONTROUTE", -1},
#endif
#ifdef SO_ERROR
			    {"SO_ERROR", static_cast<long>(SO_ERROR)},
#else
			    {"SO_ERROR", -1},
#endif
#ifdef SO_KEEPALIVE
			    {"SO_KEEPALIVE", static_cast<long>(SO_KEEPALIVE)},
#else
			    {"SO_KEEPALIVE", -1},
#endif
#ifdef SO_LINGER
			    {"SO_LINGER", static_cast<long>(SO_LINGER)},
#else
			    {"SO_LINGER", -1},
#endif
#ifdef SO_OOBINLINE
			    {"SO_OOBINLINE", static_cast<long>(SO_OOBINLINE)},
#else
			    {"SO_OOBINLINE", -1},
#endif
#ifdef SO_PASSCRED
			    {"SO_PASSCRED", static_cast<long>(SO_PASSCRED)},
#else
			    {"SO_PASSCRED", -1},
#endif
#ifdef SO_PEERCRED
			    {"SO_PEERCRED", static_cast<long>(SO_PEERCRED)},
#else
			    {"SO_PEERCRED", -1},
#endif
#ifdef SO_PROTOCOL
			    {"SO_PROTOCOL", static_cast<long>(SO_PROTOCOL)},
#else
			    {"SO_PROTOCOL", -1},
#endif
#ifdef SO_RCVBUF
			    {"SO_RCVBUF", static_cast<long>(SO_RCVBUF)},
#else
			    {"SO_RCVBUF", -1},
#endif
#ifdef SO_RCVLOWAT
			    {"SO_RCVLOWAT", static_cast<long>(SO_RCVLOWAT)},
#else
			    {"SO_RCVLOWAT", -1},
#endif
#ifdef SO_RCVTIMEO
			    {"SO_RCVTIMEO", static_cast<long>(SO_RCVTIMEO)},
#else
			    {"SO_RCVTIMEO", -1},
#endif
#ifdef SO_REUSEADDR
			    {"SO_REUSEADDR", static_cast<long>(SO_REUSEADDR)},
#else
			    {"SO_REUSEADDR", -1},
#endif
#ifdef SO_SNDBUF
			    {"SO_SNDBUF", static_cast<long>(SO_SNDBUF)},
#else
			    {"SO_SNDBUF", -1},
#endif
#ifdef SO_SNDLOWAT
			    {"SO_SNDLOWAT", static_cast<long>(SO_SNDLOWAT)},
#else
			    {"SO_SNDLOWAT", -1},
#endif
#ifdef SO_SNDTIMEO
			    {"SO_SNDTIMEO", static_cast<long>(SO_SNDTIMEO)},
#else
			    {"SO_SNDTIMEO", -1},
#endif
#ifdef SO_TYPE
			    {"SO_TYPE", static_cast<long>(SO_TYPE)},
#else
			    {"SO_TYPE", -1},
#endif
#ifdef SPLICE_F_MOVE
			    {"SPLICE_F_MOVE", static_cast<long>(SPLICE_F_MOVE)},
#else
			    {"SPLICE_F_MOVE", -1},
#endif
#ifdef SPLICE_F_NONBLOCK
			    {"SPLICE_F_NONBLOCK", static_cast<long>(SPLICE_F_NONBLOCK)},
#else
			    {"SPLICE_F_NONBLOCK", -1},
#endif
#ifdef SPLICE_F_MORE
			    {"SPLICE_F_MORE", static_cast<long>(SPLICE_F_MORE)},
#else
			    {"SPLICE_F_MORE", -1},
#endif
#ifdef STDERR_FILENO
			    {"STDERR_FILENO", static_cast<long>(STDERR_FILENO)},
#else
			    {"STDERR_FILENO", -1},
#endif
#ifdef STDIN_FILENO
			    {"STDIN_FILENO", static_cast<long>(STDIN_FILENO)},
#else
			    {"STDIN_FILENO", -1},
#endif
#ifdef STDOUT_FILENO
			    {"STDOUT_FILENO", static_cast<long>(STDOUT_FILENO)},
#else
			    {"STDOUT_FILENO", -1},
#endif
#ifdef ST_MANDLOCK
			    {"ST_MANDLOCK", static_cast<long>(ST_MANDLOCK)},
#else
			    {"ST_MANDLOCK", -1},
#endif
#ifdef ST_NOATIME
			    {"ST_NOATIME", static_cast<long>(ST_NOATIME)},
#else
			    {"ST_NOATIME", -1},
#endif
#ifdef ST_NODEV
			    {"ST_NODEV", static_cast<long>(ST_NODEV)},
#else
			    {"ST_NODEV", -1},
#endif
#ifdef ST_NODIRATIME
			    {"ST_NODIRATIME", static_cast<long>(ST_NODIRATIME)},
#else
			    {"ST_NODIRATIME", -1},
#endif
#ifdef ST_NOEXEC
			    {"ST_NOEXEC", static_cast<long>(ST_NOEXEC)},
#else
			    {"ST_NOEXEC", -1},
#endif
#ifdef ST_NOSUID
			    {"ST_NOSUID", static_cast<long>(ST_NOSUID)},
#else
			    {"ST_NOSUID", -1},
#endif
#ifdef ST_RDONLY
			    {"ST_RDONLY", static_cast<long>(ST_RDONLY)},
#else
			    {"ST_RDONLY", -1},
#endif
#ifdef ST_RELATIME
			    {"ST_RELATIME", static_cast<long>(ST_RELATIME)},
#else
			    {"ST_RELATIME", -1},
#endif
#ifdef ST_SYNCHRONOUS
			    {"ST_SYNCHRONOUS", static_cast<long>(ST_SYNCHRONOUS)},
#else
			    {"ST_SYNCHRONOUS", -1},
#endif
#ifdef S_IFBLK
			    {"S_IFBLK", static_cast<long>(S_IFBLK)},
#else
			    {"S_IFBLK", -1},
#endif
#ifdef S_IFCHR
			    {"S_IFCHR", static_cast<long>(S_IFCHR)},
#else
			    {"S_IFCHR", -1},
#endif
#ifdef S_IFDIR
			    {"S_IFDIR", static_cast<long>(S_IFDIR)},
#else
			    {"S_IFDIR", -1},
#endif
#ifdef S_IFIFO
			    {"S_IFIFO", static_cast<long>(S_IFIFO)},
#else
			    {"S_IFIFO", -1},
#endif
#ifdef S_IFLNK
			    {"S_IFLNK", static_cast<long>(S_IFLNK)},
#else
			    {"S_IFLNK", -1},
#endif
#ifdef S_IFMT
			    {"S_IFMT", static_cast<long>(S_IFMT)},
#else
			    {"S_IFMT", -1},
#endif
#ifdef S_IFREG
			    {"S_IFREG", static_cast<long>(S_IFREG)},
#else
			    {"S_IFREG", -1},
#endif
#ifdef S_IFSOCK
			    {"S_IFSOCK", static_cast<long>(S_IFSOCK)},
#else
			    {"S_IFSOCK", -1},
#endif
#ifdef S_IRGRP
			    {"S_IRGRP", static_cast<long>(S_IRGRP)},
#else
			    {"S_IRGRP", -1},
#endif
#ifdef S_IROTH
			    {"S_IROTH", static_cast<long>(S_IROTH)},
#else
			    {"S_IROTH", -1},
#endif
#ifdef S_IRUSR
			    {"S_IRUSR", static_cast<long>(S_IRUSR)},
#else
			    {"S_IRUSR", -1},
#endif
#ifdef S_IRWXG
			    {"S_IRWXG", static_cast<long>(S_IRWXG)},
#else
			    {"S_IRWXG", -1},
#endif
#ifdef S_IRWXO
			    {"S_IRWXO", static_cast<long>(S_IRWXO)},
#else
			    {"S_IRWXO", -1},
#endif
#ifdef S_IRWXU
			    {"S_IRWXU", static_cast<long>(S_IRWXU)},
#else
			    {"S_IRWXU", -1},
#endif
#ifdef S_ISGID
			    {"S_ISGID", static_cast<long>(S_ISGID)},
#else
			    {"S_ISGID", -1},
#endif
#ifdef S_ISUID
			    {"S_ISUID", static_cast<long>(S_ISUID)},
#else
			    {"S_ISUID", -1},
#endif
#ifdef S_ISVTX
			    {"S_ISVTX", static_cast<long>(S_ISVTX)},
#else
			    {"S_ISVTX", -1},
#endif
#ifdef S_IWGRP
			    {"S_IWGRP", static_cast<long>(S_IWGRP)},
#else
			    {"S_IWGRP", -1},
#endif
#ifdef S_IWOTH
			    {"S_IWOTH", static_cast<long>(S_IWOTH)},
#else
			    {"S_IWOTH", -1},
#endif
#ifdef S_IWUSR
			    {"S_IWUSR", static_cast<long>(S_IWUSR)},
#else
			    {"S_IWUSR", -1},
#endif
#ifdef S_IXGRP
			    {"S_IXGRP", static_cast<long>(S_IXGRP)},
#else
			    {"S_IXGRP", -1},
#endif
#ifdef S_IXOTH
			    {"S_IXOTH", static_cast<long>(S_IXOTH)},
#else
			    {"S_IXOTH", -1},
#endif
#ifdef S_IXUSR
			    {"S_IXUSR", static_cast<long>(S_IXUSR)},
#else
			    {"S_IXUSR", -1},
#endif
#ifdef TCP_NODELAY
			    {"TCP_NODELAY", static_cast<long>(TCP_NODELAY)},
#else
			    {"TCP_NODELAY", -1},
#endif
#ifdef TCP_USER_TIMEOUT
			    {"TCP_USER_TIMEOUT", static_cast<long>(TCP_USER_TIMEOUT)},
#else
			    {"TCP_USER_TIMEOUT", -1},
#endif
#ifdef UDP_GRO
			    {"UDP_GRO", static_cast<long>(UDP_GRO)},
#else
			    {"UDP_GRO", -1},
#endif
#ifdef UDP_SEGMENT
			    {"UDP_SEGMENT", static_cast<long>(UDP_SEGMENT)},
#else
			    {"UDP_SEGMENT", -1},
#endif
#ifdef TIOCOUTQ
			    {"TIOCOUTQ", static_cast<long>(TIOCOUTQ)},
#else
			    {"TIOCOUTQ", -1},
#endif
#ifdef UDP_ENCAP
			    {"UDP_ENCAP", static_cast<long>(UDP_ENCAP)},
#else
			    {"UDP_ENCAP", -1},
#endif
#ifdef UDP_ENCAP_ESPINUDP_NON_IKE
			    {"UDP_ENCAP_ESPINUDP_NON_IKE", static_cast<long>(UDP_ENCAP_ESPINUDP_NON_IKE)},
#else
			    {"UDP_ENCAP_ESPINUDP_NON_IKE", -1},
#endif
#ifdef UDP_ENCAP_ESPINUDP
			    {"UDP_ENCAP_ESPINUDP", static_cast<long>(UDP_ENCAP_ESPINUDP)},
#else
			    {"UDP_ENCAP_ESPINUDP", -1},
#endif
#ifdef UNIX_PATH_MAX
			    {"UNIX_PATH_MAX", static_cast<long>(UNIX_PATH_MAX)},
#else
			    {"UNIX_PATH_MAX", -1},
#endif
#ifdef WCONTINUED
			    {"WCONTINUED", static_cast<long>(WCONTINUED)},
#else
			    {"WCONTINUED", -1},
#endif
#ifdef WEXITED
			    {"WEXITED", static_cast<long>(WEXITED)},
#else
			    {"WEXITED", -1},
#endif
#ifdef WNOHANG
			    {"WNOHANG", static_cast<long>(WNOHANG)},
#else
			    {"WNOHANG", -1},
#endif
#ifdef WNOWAIT
			    {"WNOWAIT", static_cast<long>(WNOWAIT)},
#else
			    {"WNOWAIT", -1},
#endif
#ifdef WSTOPPED
			    {"WSTOPPED", static_cast<long>(WSTOPPED)},
#else
			    {"WSTOPPED", -1},
#endif
#ifdef WUNTRACED
			    {"WUNTRACED", static_cast<long>(WUNTRACED)},
#else
			    {"WUNTRACED", -1},
#endif
#ifdef W_OK
			    {"W_OK", static_cast<long>(W_OK)},
#else
			    {"W_OK", -1},
#endif
#ifdef XATTR_CREATE
			    {"XATTR_CREATE", static_cast<long>(XATTR_CREATE)},
#else
			    {"XATTR_CREATE", -1},
#endif
#ifdef XATTR_REPLACE
			    {"XATTR_REPLACE", static_cast<long>(XATTR_REPLACE)},
#else
			    {"XATTR_REPLACE", -1},
#endif
#ifdef X_OK
			    {"X_OK", static_cast<long>(X_OK)},
#else
			    {"X_OK", -1},
#endif
#ifdef _SC_2_CHAR_TERM
			    {"_SC_2_CHAR_TERM", static_cast<long>(_SC_2_CHAR_TERM)},
#else
			    {"_SC_2_CHAR_TERM", -1},
#endif
#ifdef _SC_2_C_BIND
			    {"_SC_2_C_BIND", static_cast<long>(_SC_2_C_BIND)},
#else
			    {"_SC_2_C_BIND", -1},
#endif
#ifdef _SC_2_C_DEV
			    {"_SC_2_C_DEV", static_cast<long>(_SC_2_C_DEV)},
#else
			    {"_SC_2_C_DEV", -1},
#endif
#ifdef _SC_2_C_VERSION
			    {"_SC_2_C_VERSION", static_cast<long>(_SC_2_C_VERSION)},
#else
			    {"_SC_2_C_VERSION", -1},
#endif
#ifdef _SC_2_FORT_DEV
			    {"_SC_2_FORT_DEV", static_cast<long>(_SC_2_FORT_DEV)},
#else
			    {"_SC_2_FORT_DEV", -1},
#endif
#ifdef _SC_2_FORT_RUN
			    {"_SC_2_FORT_RUN", static_cast<long>(_SC_2_FORT_RUN)},
#else
			    {"_SC_2_FORT_RUN", -1},
#endif
#ifdef _SC_2_LOCALEDEF
			    {"_SC_2_LOCALEDEF", static_cast<long>(_SC_2_LOCALEDEF)},
#else
			    {"_SC_2_LOCALEDEF", -1},
#endif
#ifdef _SC_2_SW_DEV
			    {"_SC_2_SW_DEV", static_cast<long>(_SC_2_SW_DEV)},
#else
			    {"_SC_2_SW_DEV", -1},
#endif
#ifdef _SC_2_UPE
			    {"_SC_2_UPE", static_cast<long>(_SC_2_UPE)},
#else
			    {"_SC_2_UPE", -1},
#endif
#ifdef _SC_2_VERSION
			    {"_SC_2_VERSION", static_cast<long>(_SC_2_VERSION)},
#else
			    {"_SC_2_VERSION", -1},
#endif
#ifdef _SC_AIO_LISTIO_MAX
			    {"_SC_AIO_LISTIO_MAX", static_cast<long>(_SC_AIO_LISTIO_MAX)},
#else
			    {"_SC_AIO_LISTIO_MAX", -1},
#endif
#ifdef _SC_AIO_MAX
			    {"_SC_AIO_MAX", static_cast<long>(_SC_AIO_MAX)},
#else
			    {"_SC_AIO_MAX", -1},
#endif
#ifdef _SC_AIO_PRIO_DELTA_MAX
			    {"_SC_AIO_PRIO_DELTA_MAX", static_cast<long>(_SC_AIO_PRIO_DELTA_MAX)},
#else
			    {"_SC_AIO_PRIO_DELTA_MAX", -1},
#endif
#ifdef _SC_ARG_MAX
			    {"_SC_ARG_MAX", static_cast<long>(_SC_ARG_MAX)},
#else
			    {"_SC_ARG_MAX", -1},
#endif
#ifdef _SC_ASYNCHRONOUS_IO
			    {"_SC_ASYNCHRONOUS_IO", static_cast<long>(_SC_ASYNCHRONOUS_IO)},
#else
			    {"_SC_ASYNCHRONOUS_IO", -1},
#endif
#ifdef _SC_ATEXIT_MAX
			    {"_SC_ATEXIT_MAX", static_cast<long>(_SC_ATEXIT_MAX)},
#else
			    {"_SC_ATEXIT_MAX", -1},
#endif
#ifdef _SC_AVPHYS_PAGES
			    {"_SC_AVPHYS_PAGES", static_cast<long>(_SC_AVPHYS_PAGES)},
#else
			    {"_SC_AVPHYS_PAGES", -1},
#endif
#ifdef _SC_BC_BASE_MAX
			    {"_SC_BC_BASE_MAX", static_cast<long>(_SC_BC_BASE_MAX)},
#else
			    {"_SC_BC_BASE_MAX", -1},
#endif
#ifdef _SC_BC_DIM_MAX
			    {"_SC_BC_DIM_MAX", static_cast<long>(_SC_BC_DIM_MAX)},
#else
			    {"_SC_BC_DIM_MAX", -1},
#endif
#ifdef _SC_BC_SCALE_MAX
			    {"_SC_BC_SCALE_MAX", static_cast<long>(_SC_BC_SCALE_MAX)},
#else
			    {"_SC_BC_SCALE_MAX", -1},
#endif
#ifdef _SC_BC_STRING_MAX
			    {"_SC_BC_STRING_MAX", static_cast<long>(_SC_BC_STRING_MAX)},
#else
			    {"_SC_BC_STRING_MAX", -1},
#endif
#ifdef _SC_CHILD_MAX
			    {"_SC_CHILD_MAX", static_cast<long>(_SC_CHILD_MAX)},
#else
			    {"_SC_CHILD_MAX", -1},
#endif
#ifdef _SC_CLK_TCK
			    {"_SC_CLK_TCK", static_cast<long>(_SC_CLK_TCK)},
#else
			    {"_SC_CLK_TCK", -1},
#endif
#ifdef _SC_COLL_WEIGHTS_MAX
			    {"_SC_COLL_WEIGHTS_MAX", static_cast<long>(_SC_COLL_WEIGHTS_MAX)},
#else
			    {"_SC_COLL_WEIGHTS_MAX", -1},
#endif
#ifdef _SC_DELAYTIMER_MAX
			    {"_SC_DELAYTIMER_MAX", static_cast<long>(_SC_DELAYTIMER_MAX)},
#else
			    {"_SC_DELAYTIMER_MAX", -1},
#endif
#ifdef _SC_EXPR_NEST_MAX
			    {"_SC_EXPR_NEST_MAX", static_cast<long>(_SC_EXPR_NEST_MAX)},
#else
			    {"_SC_EXPR_NEST_MAX", -1},
#endif
#ifdef _SC_FSYNC
			    {"_SC_FSYNC", static_cast<long>(_SC_FSYNC)},
#else
			    {"_SC_FSYNC", -1},
#endif
#ifdef _SC_GETGR_R_SIZE_MAX
			    {"_SC_GETGR_R_SIZE_MAX", static_cast<long>(_SC_GETGR_R_SIZE_MAX)},
#else
			    {"_SC_GETGR_R_SIZE_MAX", -1},
#endif
#ifdef _SC_GETPW_R_SIZE_MAX
			    {"_SC_GETPW_R_SIZE_MAX", static_cast<long>(_SC_GETPW_R_SIZE_MAX)},
#else
			    {"_SC_GETPW_R_SIZE_MAX", -1},
#endif
#ifdef _SC_IOV_MAX
			    {"_SC_IOV_MAX", static_cast<long>(_SC_IOV_MAX)},
#else
			    {"_SC_IOV_MAX", -1},
#endif
#ifdef _SC_JOB_CONTROL
			    {"_SC_JOB_CONTROL", static_cast<long>(_SC_JOB_CONTROL)},
#else
			    {"_SC_JOB_CONTROL", -1},
#endif
#ifdef _SC_LINE_MAX
			    {"_SC_LINE_MAX", static_cast<long>(_SC_LINE_MAX)},
#else
			    {"_SC_LINE_MAX", -1},
#endif
#ifdef _SC_LOGIN_NAME_MAX
			    {"_SC_LOGIN_NAME_MAX", static_cast<long>(_SC_LOGIN_NAME_MAX)},
#else
			    {"_SC_LOGIN_NAME_MAX", -1},
#endif
#ifdef _SC_MAPPED_FILES
			    {"_SC_MAPPED_FILES", static_cast<long>(_SC_MAPPED_FILES)},
#else
			    {"_SC_MAPPED_FILES", -1},
#endif
#ifdef _SC_MEMLOCK
			    {"_SC_MEMLOCK", static_cast<long>(_SC_MEMLOCK)},
#else
			    {"_SC_MEMLOCK", -1},
#endif
#ifdef _SC_MEMLOCK_RANGE
			    {"_SC_MEMLOCK_RANGE", static_cast<long>(_SC_MEMLOCK_RANGE)},
#else
			    {"_SC_MEMLOCK_RANGE", -1},
#endif
#ifdef _SC_MEMORY_PROTECTION
			    {"_SC_MEMORY_PROTECTION", static_cast<long>(_SC_MEMORY_PROTECTION)},
#else
			    {"_SC_MEMORY_PROTECTION", -1},
#endif
#ifdef _SC_MESSAGE_PASSING
			    {"_SC_MESSAGE_PASSING", static_cast<long>(_SC_MESSAGE_PASSING)},
#else
			    {"_SC_MESSAGE_PASSING", -1},
#endif
#ifdef _SC_MQ_OPEN_MAX
			    {"_SC_MQ_OPEN_MAX", static_cast<long>(_SC_MQ_OPEN_MAX)},
#else
			    {"_SC_MQ_OPEN_MAX", -1},
#endif
#ifdef _SC_MQ_PRIO_MAX
			    {"_SC_MQ_PRIO_MAX", static_cast<long>(_SC_MQ_PRIO_MAX)},
#else
			    {"_SC_MQ_PRIO_MAX", -1},
#endif
#ifdef _SC_NGROUPS_MAX
			    {"_SC_NGROUPS_MAX", static_cast<long>(_SC_NGROUPS_MAX)},
#else
			    {"_SC_NGROUPS_MAX", -1},
#endif
#ifdef _SC_NPROCESSORS_CONF
			    {"_SC_NPROCESSORS_CONF", static_cast<long>(_SC_NPROCESSORS_CONF)},
#else
			    {"_SC_NPROCESSORS_CONF", -1},
#endif
#ifdef _SC_NPROCESSORS_ONLN
			    {"_SC_NPROCESSORS_ONLN", static_cast<long>(_SC_NPROCESSORS_ONLN)},
#else
			    {"_SC_NPROCESSORS_ONLN", -1},
#endif
#ifdef _SC_OPEN_MAX
			    {"_SC_OPEN_MAX", static_cast<long>(_SC_OPEN_MAX)},
#else
			    {"_SC_OPEN_MAX", -1},
#endif
#ifdef _SC_PAGESIZE
			    {"_SC_PAGESIZE", static_cast<long>(_SC_PAGESIZE)},
#else
			    {"_SC_PAGESIZE", -1},
#endif
#ifdef _SC_PAGE_SIZE
			    {"_SC_PAGE_SIZE", static_cast<long>(_SC_PAGE_SIZE)},
#else
			    {"_SC_PAGE_SIZE", -1},
#endif
#ifdef _SC_PASS_MAX
			    {"_SC_PASS_MAX", static_cast<long>(_SC_PASS_MAX)},
#else
			    {"_SC_PASS_MAX", -1},
#endif
#ifdef _SC_PHYS_PAGES
			    {"_SC_PHYS_PAGES", static_cast<long>(_SC_PHYS_PAGES)},
#else
			    {"_SC_PHYS_PAGES", -1},
#endif
#ifdef _SC_PRIORITIZED_IO
			    {"_SC_PRIORITIZED_IO", static_cast<long>(_SC_PRIORITIZED_IO)},
#else
			    {"_SC_PRIORITIZED_IO", -1},
#endif
#ifdef _SC_PRIORITY_SCHEDULING
			    {"_SC_PRIORITY_SCHEDULING", static_cast<long>(_SC_PRIORITY_SCHEDULING)},
#else
			    {"_SC_PRIORITY_SCHEDULING", -1},
#endif
#ifdef _SC_REALTIME_SIGNALS
			    {"_SC_REALTIME_SIGNALS", static_cast<long>(_SC_REALTIME_SIGNALS)},
#else
			    {"_SC_REALTIME_SIGNALS", -1},
#endif
#ifdef _SC_RE_DUP_MAX
			    {"_SC_RE_DUP_MAX", static_cast<long>(_SC_RE_DUP_MAX)},
#else
			    {"_SC_RE_DUP_MAX", -1},
#endif
#ifdef _SC_RTSIG_MAX
			    {"_SC_RTSIG_MAX", static_cast<long>(_SC_RTSIG_MAX)},
#else
			    {"_SC_RTSIG_MAX", -1},
#endif
#ifdef _SC_SAVED_IDS
			    {"_SC_SAVED_IDS", static_cast<long>(_SC_SAVED_IDS)},
#else
			    {"_SC_SAVED_IDS", -1},
#endif
#ifdef _SC_SEMAPHORES
			    {"_SC_SEMAPHORES", static_cast<long>(_SC_SEMAPHORES)},
#else
			    {"_SC_SEMAPHORES", -1},
#endif
#ifdef _SC_SEM_NSEMS_MAX
			    {"_SC_SEM_NSEMS_MAX", static_cast<long>(_SC_SEM_NSEMS_MAX)},
#else
			    {"_SC_SEM_NSEMS_MAX", -1},
#endif
#ifdef _SC_SEM_VALUE_MAX
			    {"_SC_SEM_VALUE_MAX", static_cast<long>(_SC_SEM_VALUE_MAX)},
#else
			    {"_SC_SEM_VALUE_MAX", -1},
#endif
#ifdef _SC_SHARED_MEMORY_OBJECTS
			    {"_SC_SHARED_MEMORY_OBJECTS", static_cast<long>(_SC_SHARED_MEMORY_OBJECTS)},
#else
			    {"_SC_SHARED_MEMORY_OBJECTS", -1},
#endif
#ifdef _SC_SIGQUEUE_MAX
			    {"_SC_SIGQUEUE_MAX", static_cast<long>(_SC_SIGQUEUE_MAX)},
#else
			    {"_SC_SIGQUEUE_MAX", -1},
#endif
#ifdef _SC_STREAM_MAX
			    {"_SC_STREAM_MAX", static_cast<long>(_SC_STREAM_MAX)},
#else
			    {"_SC_STREAM_MAX", -1},
#endif
#ifdef _SC_SYNCHRONIZED_IO
			    {"_SC_SYNCHRONIZED_IO", static_cast<long>(_SC_SYNCHRONIZED_IO)},
#else
			    {"_SC_SYNCHRONIZED_IO", -1},
#endif
#ifdef _SC_THREADS
			    {"_SC_THREADS", static_cast<long>(_SC_THREADS)},
#else
			    {"_SC_THREADS", -1},
#endif
#ifdef _SC_THREAD_ATTR_STACKADDR
			    {"_SC_THREAD_ATTR_STACKADDR", static_cast<long>(_SC_THREAD_ATTR_STACKADDR)},
#else
			    {"_SC_THREAD_ATTR_STACKADDR", -1},
#endif
#ifdef _SC_THREAD_ATTR_STACKSIZE
			    {"_SC_THREAD_ATTR_STACKSIZE", static_cast<long>(_SC_THREAD_ATTR_STACKSIZE)},
#else
			    {"_SC_THREAD_ATTR_STACKSIZE", -1},
#endif
#ifdef _SC_THREAD_DESTRUCTOR_ITERATIONS
			    {"_SC_THREAD_DESTRUCTOR_ITERATIONS", static_cast<long>(_SC_THREAD_DESTRUCTOR_ITERATIONS)},
#else
			    {"_SC_THREAD_DESTRUCTOR_ITERATIONS", -1},
#endif
#ifdef _SC_THREAD_KEYS_MAX
			    {"_SC_THREAD_KEYS_MAX", static_cast<long>(_SC_THREAD_KEYS_MAX)},
#else
			    {"_SC_THREAD_KEYS_MAX", -1},
#endif
#ifdef _SC_THREAD_PRIORITY_SCHEDULING
			    {"_SC_THREAD_PRIORITY_SCHEDULING", static_cast<long>(_SC_THREAD_PRIORITY_SCHEDULING)},
#else
			    {"_SC_THREAD_PRIORITY_SCHEDULING", -1},
#endif
#ifdef _SC_THREAD_PRIO_INHERIT
			    {"_SC_THREAD_PRIO_INHERIT", static_cast<long>(_SC_THREAD_PRIO_INHERIT)},
#else
			    {"_SC_THREAD_PRIO_INHERIT", -1},
#endif
#ifdef _SC_THREAD_PRIO_PROTECT
			    {"_SC_THREAD_PRIO_PROTECT", static_cast<long>(_SC_THREAD_PRIO_PROTECT)},
#else
			    {"_SC_THREAD_PRIO_PROTECT", -1},
#endif
#ifdef _SC_THREAD_SAFE_FUNCTIONS
			    {"_SC_THREAD_SAFE_FUNCTIONS", static_cast<long>(_SC_THREAD_SAFE_FUNCTIONS)},
#else
			    {"_SC_THREAD_SAFE_FUNCTIONS", -1},
#endif
#ifdef _SC_THREAD_STACK_MIN
			    {"_SC_THREAD_STACK_MIN", static_cast<long>(_SC_THREAD_STACK_MIN)},
#else
			    {"_SC_THREAD_STACK_MIN", -1},
#endif
#ifdef _SC_THREAD_THREADS_MAX
			    {"_SC_THREAD_THREADS_MAX", static_cast<long>(_SC_THREAD_THREADS_MAX)},
#else
			    {"_SC_THREAD_THREADS_MAX", -1},
#endif
#ifdef _SC_TIMERS
			    {"_SC_TIMERS", static_cast<long>(_SC_TIMERS)},
#else
			    {"_SC_TIMERS", -1},
#endif
#ifdef _SC_TIMER_MAX
			    {"_SC_TIMER_MAX", static_cast<long>(_SC_TIMER_MAX)},
#else
			    {"_SC_TIMER_MAX", -1},
#endif
#ifdef _SC_TTY_NAME_MAX
			    {"_SC_TTY_NAME_MAX", static_cast<long>(_SC_TTY_NAME_MAX)},
#else
			    {"_SC_TTY_NAME_MAX", -1},
#endif
#ifdef _SC_TZNAME_MAX
			    {"_SC_TZNAME_MAX", static_cast<long>(_SC_TZNAME_MAX)},
#else
			    {"_SC_TZNAME_MAX", -1},
#endif
#ifdef _SC_VERSION
			    {"_SC_VERSION", static_cast<long>(_SC_VERSION)},
#else
			    {"_SC_VERSION", -1},
#endif
#ifdef _SC_XBS5_ILP32_OFF32
			    {"_SC_XBS5_ILP32_OFF32", static_cast<long>(_SC_XBS5_ILP32_OFF32)},
#else
			    {"_SC_XBS5_ILP32_OFF32", -1},
#endif
#ifdef _SC_XBS5_ILP32_OFFBIG
			    {"_SC_XBS5_ILP32_OFFBIG", static_cast<long>(_SC_XBS5_ILP32_OFFBIG)},
#else
			    {"_SC_XBS5_ILP32_OFFBIG", -1},
#endif
#ifdef _SC_XBS5_LP64_OFF64
			    {"_SC_XBS5_LP64_OFF64", static_cast<long>(_SC_XBS5_LP64_OFF64)},
#else
			    {"_SC_XBS5_LP64_OFF64", -1},
#endif
#ifdef _SC_XBS5_LPBIG_OFFBIG
			    {"_SC_XBS5_LPBIG_OFFBIG", static_cast<long>(_SC_XBS5_LPBIG_OFFBIG)},
#else
			    {"_SC_XBS5_LPBIG_OFFBIG", -1},
#endif
#ifdef _SC_XOPEN_CRYPT
			    {"_SC_XOPEN_CRYPT", static_cast<long>(_SC_XOPEN_CRYPT)},
#else
			    {"_SC_XOPEN_CRYPT", -1},
#endif
#ifdef _SC_XOPEN_ENH_I18N
			    {"_SC_XOPEN_ENH_I18N", static_cast<long>(_SC_XOPEN_ENH_I18N)},
#else
			    {"_SC_XOPEN_ENH_I18N", -1},
#endif
#ifdef _SC_XOPEN_LEGACY
			    {"_SC_XOPEN_LEGACY", static_cast<long>(_SC_XOPEN_LEGACY)},
#else
			    {"_SC_XOPEN_LEGACY", -1},
#endif
#ifdef _SC_XOPEN_REALTIME
			    {"_SC_XOPEN_REALTIME", static_cast<long>(_SC_XOPEN_REALTIME)},
#else
			    {"_SC_XOPEN_REALTIME", -1},
#endif
#ifdef _SC_XOPEN_REALTIME_THREADS
			    {"_SC_XOPEN_REALTIME_THREADS", static_cast<long>(_SC_XOPEN_REALTIME_THREADS)},
#else
			    {"_SC_XOPEN_REALTIME_THREADS", -1},
#endif
#ifdef _SC_XOPEN_SHM
			    {"_SC_XOPEN_SHM", static_cast<long>(_SC_XOPEN_SHM)},
#else
			    {"_SC_XOPEN_SHM", -1},
#endif
#ifdef _SC_XOPEN_UNIX
			    {"_SC_XOPEN_UNIX", static_cast<long>(_SC_XOPEN_UNIX)},
#else
			    {"_SC_XOPEN_UNIX", -1},
#endif
#ifdef _SC_XOPEN_VERSION
			    {"_SC_XOPEN_VERSION", static_cast<long>(_SC_XOPEN_VERSION)},
#else
			    {"_SC_XOPEN_VERSION", -1},
#endif
#ifdef _SC_XOPEN_XCU_VERSION
			    {"_SC_XOPEN_XCU_VERSION", static_cast<long>(_SC_XOPEN_XCU_VERSION)},
#else
			    {"_SC_XOPEN_XCU_VERSION", -1},
#endif
			};
			return table;
		}
	}  // namespace osconst
}  // namespace sandvik
