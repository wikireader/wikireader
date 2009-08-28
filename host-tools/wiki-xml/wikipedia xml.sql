-- drop table entries;
-- drop table bigram;

create table entries ( 			/* including articles, redirected articles and templates */
	seq bigint(15) not null auto_increment primary key,
	entry_type tinyint, 		/* 0 - article, 1 - redirected article, 2 - other */
	title varchar(255) COLLATE latin1_bin,
	title_search varchar(50), 	/* title for search (keeping only alphanumeric characters) */
	idx int, 			/* unique sequential index for articles */
	redirect_title varchar(255) COLLATE latin1_bin, /* title to rediret to */
	redirect_title_idx bigint(15),	/* idx for title to redirect to */
	text_start_offset bigint(15),	/* offset to the wiki XML file */
	text_len int default 0		/* article text length in the wiki XML file */
) MAX_ROWS=10000000, AVG_ROW_LENGTH=120;

create table bigram (
	seq int not null auto_increment primary key,
	bigram_chars varchar(2) COLLATE latin1_bin,
	occurrences int default 0
);

create index idx_entries_idx on entries (
	idx
);

create index idx_entries_title on entries (
	title
);

create index idx_entries_search on entries (
	title_search
);

create index idx_entries_redirect on entries (
	redirect_title
);

create index idx_bigram_chars on bigram (
	bigram_chars
);
