/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 1mthe0wl </var/spool/mail/evil>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/15 09:57:44 by 1mthe0wl          #+#    #+#             */
/*   Updated: 2021/12/23 13:30:56 by hsabir           ###   ########.fr       */
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
	char	*path;
	int		i;
	char	**split;
	char	*full_path;
	char	*full_cmd;
	i = 0;
	path = getenv("PATH");
	split = ft_split(path, ':');
	while (split[i])
	{
		full_path = ft_strjoin(split[i], "/");
		if (check_env(full_path, cmd))
		{
			full_cmd = ft_strjoin(full_path, cmd);
			exec_path_cmd(full_cmd, shell);
			free(split);
			free(full_path);
			free(full_cmd);
			break ;
		}
		free(split[i]);
		free(full_path);
		i++;
	}
	return (NULL);
}
int	builtin(t_shell *shell)
{
	int i;

	i = 0;
	while (shell->cmd->token[i])
	{
		if (!ft_strncmp(shell->cmd->token[i],"env",3))
			return(ft_env(shell));
		else if (!ft_strncmp(shell->cmd->token[i],"echo",4))
			return (ft_echo(shell->cmd->token + i));
		else if (!ft_strncmp(shell->cmd->token[i],"cd",2))
			return (ft_cd(shell->cmd->token + 1, shell));
		else if (!ft_strncmp(shell->cmd->token[i],"export",6))
			return (ft_export(shell->cmd->token, shell));
		i++;
	}
	return (0);
}
void	get_cmd(t_shell *shell)
{
	unsigned int i;
	unsigned int a;

	a = 0;
	i = 0;

	if (!builtin(shell))
		check_cmd(shell->cmd->token[0], shell);
}
void	parsing(t_shell *shell)
{
	int		status;
	t_cmd	*ptr;

	status = 0;
	ptr = shell->cmd;
	while (shell->cmd)
	{
		if (!shell->cmd->next)
		{
			get_cmd(shell);
			status = -1;
		}
		else
		{
			printf("pipe test\n");
			get_pipes(shell);
		}
		shell->cmd = shell->cmd->next;
	}
	//if (shell->n_pipes == 1)
	//	printf("Hello pipes\n");
	//if (!builtin(shell))
	//	printf("Error\n");
	/*if (shell->n_pipes)
	{
		if (shell->pipe_flag == 1)
		{
		//	parse_pipes(shell);
		}
	}
	else
	{
		get_cmd(shell);
	}
	return (1);*/
}
