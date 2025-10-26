To start clone the repo to your local machine and navigate to the directory it creates.

```console
$ git clone {link to remote repo}
$ cd {repo_name}
```

Create a branch for development

```console
$ git checkoout -b {branch_name}
```

After making changes, stage and commit them.

```console
$ git add {path_to_changes}
$ git commit -m "descriptive message in imperative form (e.g. Adds support)"
```

Push changes to the remote repository

```console
$ git push origin {branch_name}
```

Finally, create a merge request targeting the main branch to integrate changes into production.

