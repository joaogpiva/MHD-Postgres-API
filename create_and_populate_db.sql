CREATE TABLE public.monkeys (
	id serial4 NOT NULL,
	"name" varchar(50) NULL,
	price int4 NULL,
	"type" bpchar(1) NULL,
	CONSTRAINT monkeys_pkey PRIMARY KEY (id)
);

INSERT INTO public.monkeys ("name",price,"type") VALUES
	 ('dart',150,'P'),
	 ('ninja',500,'M'),
	 ('tack',350,'P'),
	 ('wizard',400,'M'),
	 ('druid',450,'M'),
	 ('bomba',600,'P'),
	 ('super',2500,'M');
