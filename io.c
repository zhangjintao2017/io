/* ex: set tabstop=4: */

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/io.h>

//	Command line interface to the IO functions in libc.
//
//		inb, inw, inl
//		outb, outw, outl
//
//
//	Syntax:
//		io <b | w | l> <port> [data]
//

#define	ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))
typedef	unsigned char	u8;
typedef	unsigned short	u16;
typedef	unsigned int	u32;


typedef struct options
{
	u32	port;	// IO port number
	int	size;	// 1 - 8-bits, 2 - 16-bits, 3 - 32-bits
	int	write;	// 1 - write, 0 - read
	u32	data;	// Write data.
} opts_t;

opts_t	opts;


void
usage (const char *cmd)
{
	fprintf (stderr, "Usage: %s <b | w | l> <port> [data]\n", cmd);
}

int
parse_args (int argc, char **argv, opts_t *opts)
{
	char	*cmd;

	cmd = argv [0];

	if ((argc < 3) || (argc > 4))
	{
		usage (cmd);
		return -1;
	}

	switch (*argv [1])
	{
		case 'b':
			opts->size = 1;
			break;
		case 'w':
			opts->size = 2;
			break;
		case 'l':
			opts->size = 4;
			break;
		default:
			usage (cmd);
			return -1;
	}

	opts->port = strtoul (argv [2], NULL, 0);
	opts->write = 0;

	if (argc == 4)
	{
		opts->write = 1;
		opts->data = strtoul (argv [3], NULL, 0);
	}

	return 0;
}

u32
io_read (opts_t *opts)
{
	switch (opts->size)
	{
		case 1:
			opts->data = inb (opts->port);
			break;
		case 2:
			opts->data = inw (opts->port);
			break;
		case 4:
			opts->data = inl (opts->port);
			break;
		default:
			break;
	}

	return opts->data;
}

void
io_write (opts_t *opts)
{
	switch (opts->size)
	{
		case 1:
			outb (opts->data, opts->port);
			break;
		case 2:
			outw (opts->data, opts->port);
			break;
		case 4:
			outl (opts->data, opts->port);
			break;
		default:
			break;
	}
}
int
main (int argc, char **argv)
{
	int	ret;
	int	i;
	unsigned char ports[] = {4, 5, 6, 7};

	if (parse_args (argc, argv, &opts))
		exit (1);

	ret = iopl (3);
	if (ret)
	{
		perror (NULL);
		fprintf (stderr, "Error iopl\n");
		exit (1);
	}

	ret = ioperm (opts.port, 2, 1);
	if (ret)
	{
		perror (NULL);
		fprintf (stderr, "Error ioperm\n");
		exit (1);
	}

	if (opts.write)
		io_write (&opts);
	else
		printf ("0x%x\n", io_read (&opts));

	ret = ioperm (opts.port, 2, 0);
	exit (0);
}
