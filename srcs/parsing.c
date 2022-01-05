/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 1mthe0wl </var/spool/mail/evil>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/15 09:57:44 by 1mthe0wl          #+#    #+#             */
/*   Updated: 2022/01/05 11:59:43 by 1mthe0wl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/minishell.h"

int	check_env(char *path, char *cmd)
{
	int	i;
	char	*ret_join;

	i = 0;
	ret_join = ft_strjoin(path, cmd);
	while (path[i])
	{
		if (!(access(ret_join, X_OK)))
		{
			free(ret_join);
			return (1);
		}
		else
			i++;
	}
	free(ret_join);
	return (0);
}

char	*check_cmd(char *cmd, t_shell *shell)
{
	char	*path_tmp;
	int		i;
	char	**split;
	char	*full_path;
	char	*full_cmd;
	i = 0;
	path_tmp = getenv("PATH");
	split = ft_split(path_tmp, ':');
	while (split[i])
	{
		full_path = ft_strjoin(split[i], "/");
		if (check_env(full_path, cmd))
		{
			full_cmd = ft_strjoin(full_path, cmd);
			//exec_path_cmd(full_cmd, shell);
			free(split);
			free(full_path);
			//free(full_cmd);
			return (full_cmd);
		}
		free(split[i]);
		free(full_path);
		i++;
	}
	return (NULL);
}

/*int	builtin(t_shell *shell, int in_fork)
{
	//int i;
	if (!in_fork)
		redirection_main(shell);
	//i = 0;
	//while (shell->cmd->token[i])
	//{
		if (!ft_strncmp(*shell->cmd->token,"env",3))
			shell->ret = ft_env(shell);
		else if (!ft_strncmp(*shell->cmd->token,"echo",4))
			shell->ret = ft_echo(shell->cmd->token + i);
		else if (!ft_strncmp(*shell->cmd->token,"cd",2))
			shell->ret = ft_cd(*shell->cmd->token + 1, shell);
		else if (!ft_strncmp(*shell->cmd->token,"export",6))
			shell->ret = ft_export(*shell->cmd->token, shell);
	//	i++;
	//}
	return (1);
}*/
/*
void	get_cmd(t_shell *shell)
{
	unsigned int i;
	unsigned int a;

	a = 0;
	i = 0;

	if (!builtin(shell))
		check_cmd(shell->cmd->token[0], shell);
}*/

int	heredoc(t_shell *shell)
{
	t_cmd	*cmd;
	int		i;

	cmd = shell->cmd;
	while (shell->cmd)
	{
		i = -1;
		if (shell->cmd->redirection)
		{
			while (shell->cmd->redirection[++i])
			{
				if (redirection_type(shell->cmd->redirection[i]) != HEREDOC)
					continue ;
				if (!parse_heredoc(shell->cmd->redirection[i + 1], shell, cmd))
					return (0);
			}
		}
		shell->cmd = shell->cmd->next;
	}
	shell->cmd = cmd;
	return (1);
}

int	launch(t_shell *shell, int *status, char **env)
{
	int	first;
	int	fd[2];

	first = 1;
	while (shell->cmd)
	{
		if (first && !shell->cmd->redirection[0] && is_builtin(*shell->cmd->token))
		{
//			printf("Nothing...\n");
			exec_built_in(shell, 1);
			*status = -1;
//			return (1);
		}
		else
		{
			if (!fd_set_in_out(shell, fd, first))
				return (0);
			exec_pipe_cmd(shell, env, fd[0]);
		}
		if (shell->cmd->next)
			close_error(fd[1]);
		if (shell->cmd->in != 0)
			close_error(shell->cmd->in);
		shell->cmd = shell->cmd->next;
		first = 0;
	}
	return (1);
}
char	*parse_dol(char *tr, t_shell *shell,char *shellvar)
{
	t_env *tmp;
	tmp = shell->env;
	if (shellvar[0] == '?')
	{
		tr = ft_itoa(shell->lreturn);
		return (tr);
	}
	else 
	{
		while (tmp)
		{
			if (!ft_strcmp(tmp->name, shellvar))
			{
				tr = ft_strdup(tmp->value);
				return (tr);
			}
			tmp = tmp->next;
		}
	}
	return (NULL);
}
void	parse_var(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->cmd->token[i])
	{
		if (ft_strchr(shell->cmd->token[i], '$'))
			shell->cmd->token[i] = parse_dol(shell->cmd->token[i], shell, shell->cmd->token[i] + 1);
		i++;
	}
}
void	parsing(t_shell *shell, char **env)
{
	int		status;
	t_cmd	*ptr;

	status = 0;
	shell->error = 0;
	ptr = shell->cmd;
//	parse_var(shell);
	if (heredoc(shell))
	{
		if (!launch(shell, &status, env))
			close_fds(ptr);
		if (status != -1 && !shell->error)
			wait_all_process(ptr, shell);
	}
	free_cmd(ptr);
	//free_envs(shell->env);
//	free_cmd(ptr);
	shell->cmd = NULL;
}
