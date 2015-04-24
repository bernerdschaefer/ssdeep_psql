Requirements (for Ubuntu 14.04 Trusty)
============

Install the package requirements:

  apt-get install -y postgresql postgresql-client postgresql-server-dev-all postgresql-contrib postgresql-client-common postgresql-common

Download and untar in a temporary directory (e.g. ~/temp) the source code of the version of PostgreSQL that you are using from [the official website](http://www.postgresql.org/ftp/source).

Installation
============

Clone `ssdeep_psql` to `~/temp/postgresql-9.4.1/contrib/`.

Go to `~/temp/postgresql-9.4.1` and run `./configure`.

Go to `~/temp/postgresql-9.4.1/contrib/ssdeep_psql`, and then:

```bash
make
make install
```
  
You should now have `/usr/lib/postgresql/9.4/lib/ssdeep_psql.so` installed. If not, have a look under `/usr/local/pgsql/lib/ssdeep_psql.so` and move the file to the appropriate address.

To expose the ssdeep functions in your database, you'll need to issue the following statements:

```SQL
CREATE OR REPLACE FUNCTION fuzzy_hash(TEXT) RETURNS TEXT AS 'ssdeep_psql.so', 'pg_fuzzy_hash' LANGUAGE C;
CREATE OR REPLACE FUNCTION fuzzy_compare(TEXT, TEXT) RETURNS INTEGER AS 'ssdeep_psql.so', 'pg_fuzzy_compare' LANGUAGE C;
CREATE OR REPLACE FUNCTION fuzzy_hash_compare(TEXT, TEXT) RETURNS INTEGER AS 'ssdeep_psql.so', 'pg_fuzzy_hash_compare' LANGUAGE C;
```

Usage
=====

`ssdeep_psql` exposes three functions for working with fuzzy hashes: `fuzzy_hash`, `fuzzy_compare`, and `fuzzy_hash_compare`:

```SQL
# SELECT fuzzy_hash('john');
 fuzzy_hash
 ------------
 3:Uvl

# SELECT fuzzy_compare('lorem ipsum dolor sit amet', 'lorem ipsum dolor sit amet');
 fuzzy_compare
 ---------------
              9

# SELECT fuzzy_hash_compare('3:JBo8MRwRn:J3PR', '3:JBo8MRwx:J3Px');
 fuzzy_hash_compare
 --------------------
                   8
```

Here's a suggested workflow for using ssdeep_psql:

```SQL
CREATE TABLE "stories" ("id" SERIAL, "body" TEXT, "hash" TEXT);

CREATE OR REPLACE FUNCTION set_fuzzy_hash() RETURNS TRIGGER AS $$
BEGIN
    NEW.hash := fuzzy_hash(NEW.body);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER "set_fuzzy_hash_for_body_on_insert" BEFORE INSERT ON "stories"
  FOR EACH ROW EXECUTE PROCEDURE set_fuzzy_hash();
CREATE TRIGGER "set_fuzzy_hash_for_body_on_update" BEFORE UPDATE ON "stories"
  FOR EACH ROW EXECUTE PROCEDURE set_fuzzy_hash();

INSERT INTO "stories" ("body") VALUES ('Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.');

INSERT INTO "stories" ("body") VALUES ('Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

SELECT id, fuzzy_compare(body, 'Lorem ipsum dolor sit amet') as score from "stories" ORDER BY fuzzy_compare(body, 'Lorem ipsum dolor sit amet') DESC LIMIT 1;
```

Or, much faster:

```SQL
SELECT id, fuzzy_hash_compare(hash, fuzzy_hash('Lorem ipsum dolor sit amet')) as score from "stories"
  ORDER BY fuzzy_hash_compare(hash, fuzzy_hash('Lorem ipsum dolor sit amet')) DESC LIMIT 1;
```

And again, assuming you've calculated your search text's hash already (with, say, ssdeep_ruby):

```SQL
SELECT id, fuzzy_hash_compare(hash, '3:f4oo8MRwRn:f4kPR') as score from "stories"
  ORDER BY fuzzy_hash_compare(hash, '3:f4oo8MRwRn:f4kPR') DESC LIMIT 1;
```
