/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 1mthe0wl </var/spool/mail/evil>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/15 21:41:20 by 1mthe0wl          #+#    #+#             */
/*   Updated: 2021/12/21 15:59:34 by lgyger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/minishell.h"

int spawn_proc (int in, int out,char ***av, char  **env)
{
	pid_t pid;
	
	pid = fork();
	if (pid == 0)
	{
		if (in != 0)
		{
			dup2 (in, 0);
			close (in);
		}
		if (out != 1)
		{
			dup2 (out, 1);
			close (out);
		}
		//printf("av[i] : %s", *av[0]);
		//return execve (*av[0],*av, env);
		return execve(*av[0], *av, env);
	}
	return (1);
}

int fork_pipes (int n, char ***av, char **env)
{
	int i;
	pid_t pid;
	int in;
	int fd [2];

	in = 0;
	i = 0;
	while (i < n)
	{
		pipe (fd);
		if (spawn_proc (in, fd [1], av + i, env) < 0)
			return (-1);
		/* the child will write here.  */

		close (fd [1]);

      /* the next child will read from there.  */
		in = fd [0];
		i++;
	}
	if (in != 0)
		dup2 (in, 0);

	//if (!(access(av[i][0], X_OK)))
	return (execve (av[i][0],av[i],NULL));
	return (1);
}

int	exec_pipe_cmd(t_shell *shell, char **env)
{
	int		fd[2];
	pid_t	pid;
	int i; 
	int status;

	i = 0;
	if (shell->pipe_cmd_exist == 1)
		pid = fork();
	if (pid == 0)
	{
		i = fork_pipes(shell->n_pipes,shell->cmds_pipe, env);
	}
	else if (pid > 0)
		waitpid(pid,&status,WCONTINUED);
	if (i < 0)
		return (0);
	shell->lreturn = WEXITSTATUS(status);
	return (1);
}

void	exec_path_cmd(char *path, t_shell *shell)
{
	int		status;
	pid_t	pid;
	pid_t	child_pid;

	status = 0;
	pid = fork();
	if (pid == 0)
	{
		execv(path, shell->tokens);
	}
	else if (pid > 0)
		child_pid = waitpid(pid, &status, 0);
}
